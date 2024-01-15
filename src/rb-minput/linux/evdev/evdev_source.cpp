// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <fstream>
#include <sstream>
#include <algorithm>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "linux/evdev/evdev_source.hpp"
#include "linux/evdev/evdev_handle.hpp"
#include "linux/evdev/evdev_device.hpp"
#include "linux/evdev/evdev.hpp"
#include "linux/udev_info.hpp"
#include "linux/fs.hpp"
#include "source.hpp"
#include "context.hpp"

namespace multi_input {
    namespace lnx {
        evdev_source::evdev_source(context* ctx) :
            source(ctx), m_device_map(), m_inotify(open_inotify()), m_inotify_udev(open_inotify()),
            m_poller(), m_sysfs_base_path(fs::sysfs_path()), m_pending()
        {
            if (inotify_add_watch(m_inotify.get(), "/dev/input", IN_CREATE | IN_DELETE) < 0) {
                throw_posix_error("Failed to add inotify watch on /dev/input");
            }

            if (inotify_add_watch(m_inotify_udev.get(), "/run/udev", IN_DELETE) < 0) {
                throw_posix_error("Failed to add inotify watch on /run/udev");
            }

            m_poller.add(m_inotify.get());
            m_poller.add(m_inotify_udev.get());
        }

        evdev_source::~evdev_source() {
        }

        void evdev_source::enum_devices() {
            RB_TRACE_ENTER();
            m_ctx->log_debug(u8"evdev: enumerating devices");

            RB_TRACE("clearing existing devices");
            // like with xinput2 device names can be reused
            // so we can't tell here what changed
            // and therefore need to discard existing state
            for (auto&& pair : m_device_map) {
                m_ctx->remove_device(pair.second);
            }

            m_device_map.clear();

            auto sysfs_devices = fs::list(m_sysfs_base_path);
            for (auto&& symbolic_name : sysfs_devices) {
                add_device(symbolic_name);
            }
        }

        void evdev_source::add_device(const std::string& symbolic_name) {
            RB_TRACE_ENTER();

            if (!boost::starts_with(symbolic_name, "event")) {
                RB_TRACE("skipping non-evdev device");
                return;
            }

            auto sysfs_path = fs::join(m_sysfs_base_path, symbolic_name);
            sysfs_path = fs::canonical(sysfs_path);

            auto dev_node_path = fs::join(sysfs_path, "dev");
            auto label_path = fs::join(fs::parent(sysfs_path), "name");

            auto dev_node = boost::trim_copy(fs::read_all(dev_node_path));
            auto label = boost::trim_copy(fs::read_all(label_path));

            udev_info info{"c" + dev_node};

            if (!info.is_tagged("ID_INPUT_JOYSTICK")) {
                RB_TRACE("skipping non-joystick device");
                return;
            }

            evdev_handle handle{fs::join("/dev/input", symbolic_name), symbolic_name, info};

            if (handle.get() == nullptr) {
                RB_TRACE("skipping inaccessible device");
                return;
            }

            auto fd = handle.get_fd();

            if (m_device_map.name_to_id(symbolic_name) != nullptr) {
                RB_TRACE("replacing existing device object");
                remove_device(symbolic_name);
            }

            RB_TRACE("creating new device object");
            auto id = m_ctx->get_next_id();
            m_ctx->add_device(std::unique_ptr<device>{new evdev_device(m_ctx, id, std::move(handle))});
            m_device_map.add(symbolic_name, fd, id);
            m_poller.add(fd);
        }

        void evdev_source::remove_device(const std::string& symbolic_name) {
            RB_TRACE_ENTER();

            auto id_ptr = m_device_map.name_to_id(symbolic_name);
            if (id_ptr == nullptr) {
                RB_TRACE("id_ptr == nullptr");
                return;
            }

            auto id = *id_ptr;
            auto fd_ptr = m_device_map.id_to_fd(id);
            assert(fd_ptr != nullptr);
            auto fd = *fd_ptr;

            RB_TRACE("removing device object");
            m_poller.remove(fd);
            m_ctx->remove_device(id);
            m_device_map.remove(symbolic_name, fd, id);
        }

        evdev_device* evdev_source::get_device(int fd) {
            RB_TRACE_ENTER();

            auto id_ptr = m_device_map.fd_to_id(fd);
            if (id_ptr == nullptr) {
                RB_TRACE("unknown device: not in device map");
                return nullptr;
            }

            auto device_ptr = static_cast<evdev_device*>(m_ctx->get_device(*id_ptr));
            if (device_ptr == nullptr) {
                RB_TRACE("unknown device: not in context");
                return nullptr;
            }

            return device_ptr;
        }

        void evdev_source::drain_events() {
            RB_TRACE_ENTER();

            if (!m_poller.poll()) {
                RB_TRACE("no events");
                return;
            }

            for (auto fd : m_poller.get_ready()) {
                if (fd == m_inotify.get()) {
                    process_inotify();
                } else if (fd == m_inotify_udev.get()) {
                    process_inotify_udev();
                } else {
                    auto device_ptr = get_device(fd);
                    if (device_ptr != nullptr) {
                        process_device(*device_ptr);
                    }
                }
            }

            for (auto&& pair : m_device_map) {
                auto id = pair.second;
                auto device_ptr = static_cast<evdev_device*>(m_ctx->get_device(id));

                assert(device_ptr != nullptr);
                if (device_ptr == nullptr) {
                    continue;
                }

                device_ptr->post_update();
            }
        }

        void evdev_source::process_inotify() {
            m_ctx->log_verbose("evdev: inotify fd ready");

            while (true) {
                std::aligned_storage_t<(sizeof(inotify_event) + NAME_MAX + 1) * 16, alignof(inotify_event)> buffer;

                auto buffer_ptr = reinterpret_cast<unsigned char*>(&buffer);
                auto length = read(m_inotify.get(), static_cast<void*>(buffer_ptr), sizeof(buffer));

                m_ctx->log_verbose("evdev: inotify length = %1%", length);

                if (length == -1 && errno != EAGAIN) {
                    throw_posix_error("Failed to process inotify event on /dev/input");
                } else if (length < 0) {
                    return;
                }

                auto ptr = buffer_ptr;
                auto end = buffer_ptr + length;
                while (ptr < end) {
                    auto event = reinterpret_cast<const inotify_event*>(ptr);
                    ptr += event->len + sizeof(inotify_event);

                    auto name = event->name;
                    auto mask = event->mask;
                    auto is_dir = (mask & IN_ISDIR) != 0;
                    auto is_create = (mask & IN_CREATE) != 0;
                    auto is_delete = (mask & IN_DELETE) != 0;

                    if (is_dir) {
                        continue;
                    }

                    if (is_create) {
                        m_ctx->log_debug("evdev: inotify IN_CREATE name = %1% mask = %2%", name, mask);
                        // we can't add_device immediately because udev needs to settle first
                        // the other inotify instance is watching DELETE on /run/udev, because
                        // when udev settles it removes /run/udev/queue
                        m_pending.push_back(name);
                    } else if (is_delete) {
                        m_ctx->log_debug("evdev: inotify IN_DELETE name = %1% mask = %2%", name, mask);
                        remove_device(name);
                    }
                }
            }
        }

        void evdev_source::process_inotify_udev() {
            std::array<char, 128> buffer;

            m_ctx->log_debug("evdev: udev settled");
            while (read(m_inotify_udev.get(), buffer.data(), buffer.size()) >= 0) {
                m_ctx->log_verbose("evdev: discarding udev inotify queue");
            }

            for (auto&& name : m_pending) {
                m_ctx->log_debug("evdev: adding pending device %1%", name);
                add_device(name);
            }

            m_pending.clear();
        }

        void evdev_source::process_device(evdev_device& dev) {
            m_ctx->log_verbose("evdev: device %1% fd ready", dev.get_id());
            auto&& handle = dev.get_handle();

            while (true) {
                input_event event{};
                auto rc = libevdev_next_event(handle.get(), LIBEVDEV_READ_FLAG_NORMAL, &event);

                if (rc == -EAGAIN || rc == LIBEVDEV_READ_STATUS_SYNC) {
                    break;
                } else if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
                    dev.update(event);
                } else if (rc < 0) {
                    throw_posix_error(
                        rc, "Failed while receiving an evdev event from device %1% (%2%)",
                        dev.get_id(), handle.get_symbolic_name()
                    );
                } else {
                    m_ctx->log_warning("evdev: libevdev_next_event returned unexpected code: %1%", rc);
                    assert(!"Impossible code path");
                    break;
                }
            }
        }
    }
}
