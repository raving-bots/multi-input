// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <stdexcept>
#include <array>
#include <unordered_map>
#include <cassert>

#include <boost/algorithm/string/predicate.hpp>

#include "linux/xi2/xi2_source.hpp"
#include "linux/xi2/xi2_device.hpp"
#include "linux/xi2/x11.hpp"
#include "linux/xi2/x11_display.hpp"
#include "linux/xi2/x11_event.hpp"
#include "linux/xi2/x11_device_query.hpp"
#include "context.hpp"

namespace multi_input {
    namespace lnx {
        xi2_source::xi2_source(context *ctx)
            : source(ctx), m_opcode(), m_first_event(), m_first_error(), m_display(), m_root_window(), m_device_map()
        {
            RB_TRACE_ENTER();

            m_root_window = XDefaultRootWindow(m_display.get());
            ctx->log_verbose(u8"x11: root window = %1%", m_root_window);

            RB_TRACE("querying XI2 opcode");
            if (!XQueryExtension(m_display.get(), "XInputExtension", &m_opcode, &m_first_event, &m_first_error)) {
                throw std::runtime_error("XQueryExtension(XInputExtension) failed");
            }

            int major = 2, minor = 2;
            RB_TRACE("querying XI2 version");
            auto result = XIQueryVersion(m_display.get(), &major, &minor);
            if (result != Success) {
                throw std::runtime_error("XInput2 2.2+ is not supported on this X11 server");
            }

            std::array<unsigned char, (XI_LASTEVENT + 7)/8> event_mask{};
            XISetMask(event_mask.data(), XI_RawKeyPress);
            XISetMask(event_mask.data(), XI_RawKeyRelease);
            XISetMask(event_mask.data(), XI_RawButtonPress);
            XISetMask(event_mask.data(), XI_RawButtonRelease);
            XISetMask(event_mask.data(), XI_RawMotion);
            XISetMask(event_mask.data(), XI_HierarchyChanged);

            std::array<XIEventMask, 1> event_masks{};
            event_masks[0].deviceid = XIAllDevices;
            event_masks[0].mask_len = event_mask.size();
            event_masks[0].mask     = event_mask.data();

            RB_TRACE("selecting XI2 events");
            result = XISelectEvents(m_display.get(), m_root_window, event_masks.data(), event_masks.size());
            if (result != Success) {
                throw std::runtime_error("Failed to register for XI2 events");
            }
        }

        xi2_source::~xi2_source() {
        }

        void xi2_source::drain_events() {
            while (has_next_event()) {
                x11_event event{m_display.get(), m_opcode};

                if (!event.is_valid()) {
                    continue;
                }

                auto type = event.get_type();
                switch (type) {
                    case XI_RawKeyPress:
                    case XI_RawKeyRelease:
                    case XI_RawButtonPress:
                    case XI_RawButtonRelease:
                    case XI_RawMotion:
                        on_device_event(event.get_data<XIRawEvent>());
                        break;
                    case XI_HierarchyChanged:
                        on_hierarchy_event(event.get_data<XIHierarchyEvent>());
                        break;
                }
            }
        }

        bool xi2_source::has_next_event() {
            auto display = m_display.get();
            auto display_fd = ConnectionNumber(display);
            timeval timeout{};
            fd_set fds{};

            FD_ZERO(&fds);
            FD_SET(display_fd, &fds);

            XFlush(display);

            if (XEventsQueued(display, QueuedAlready)) {
                return true;
            } else if (select(display_fd + 1, &fds, nullptr, nullptr, &timeout) == 1) {
                return XPending(display) > 0;
            } else {
                return false;
            }
        }

        template <typename T>
        bool is_slave_device(T& info) {
            switch (info.use) {
                case XISlavePointer:
                case XISlaveKeyboard:
                case XIFloatingSlave:
                    return true;
                default:
                    return false;
            }
        }

        bool is_interesting(XIDeviceInfo& info) {
            // skip non-slave devices
            if (!is_slave_device(info)) {
                return false;
            }

            // and XTEST devices
            if (boost::istarts_with(info.name, u8"virtual core xtest")) {
                return false;
            }

            return true;
        }

        void xi2_source::enum_devices() {
            RB_TRACE_ENTER();

            m_ctx->log_debug(u8"xi2: enumerating devices");

            RB_TRACE("clearing existing devices");
            // destroy all devices if we have any
            // XI2 reuses device IDs, so we can't reliably tell
            // which ones might have changed from here (device map
            // will be instead updated in each on_hierarchy_change)
            for (auto&& pair : m_device_map) {
                m_ctx->remove_device(pair.second);
            }

            m_device_map.clear();

            RB_TRACE("fetching present devices");
            x11_device_query devices{m_display.get(), XIAllDevices};
            for (auto&& device_info : devices) {
                add_device(device_info);
            }
        }

        xi2_device* xi2_source::get_device(int x11_id) {
            RB_TRACE_ENTER();

            auto it = m_device_map.find(x11_id);
            if (it == m_device_map.end()) {
                RB_TRACE("unknown device: not in m_device_map");
                return nullptr;
            }

            auto device_ptr = static_cast<xi2_device*>(m_ctx->get_device(it->second));
            if (device_ptr == nullptr) {
                RB_TRACE("unknown device: not in context devices");
                return nullptr;
            }

            return device_ptr;
        }

        void xi2_source::on_device_event(XIRawEvent* data) {
            auto device_ptr = get_device(data->deviceid);
            if (device_ptr == nullptr) {
                return;
            }

            device_ptr->update(*data);
        }

        void xi2_source::add_device(XIDeviceInfo& info) {
            RB_TRACE_ENTER();

            if (!is_interesting(info)) {
                RB_TRACE("skipping uninteresting device");
                return;
            }

            if (m_device_map.find(info.deviceid) != m_device_map.end()) {
                RB_TRACE("replacing existing device object");
                remove_device(info.deviceid);
            }

            RB_TRACE("creating new device object");
            auto id = m_ctx->get_next_id();
            m_ctx->add_device(std::unique_ptr<device>{new xi2_device(m_ctx, id, m_display.get(), info)});
            m_device_map.emplace(info.deviceid, id);
        }

        void xi2_source::remove_device(int x11_id) {
            RB_TRACE_ENTER();

            auto map_it = m_device_map.find(x11_id);
            if (map_it == m_device_map.end()) {
                RB_TRACE("map_it == end");
                return;
            }

            RB_TRACE("removing device object");
            m_ctx->remove_device(map_it->second);
            m_device_map.erase(map_it);
        }

        void xi2_source::on_hierarchy_event(XIHierarchyEvent* data) {
            RB_TRACE_ENTER();

            m_ctx->log_debug(u8"xi2_source: on_hierarchy_event");

            for (int idx = 0; idx < data->num_info; ++idx) {
                auto&& info = data->info[idx];
                auto x11_id = info.deviceid;
                auto flags  = info.flags;

                if (!is_slave_device(info)) {
                    m_ctx->log_verbose(u8"xi2_source: device %1% (index %2%) is not a slave device (%3%)", x11_id, idx, info.use);
                    RB_TRACE("skipping master device");
                    continue;
                }

                m_ctx->log_verbose(u8"xi2_source: device %1% flags %2%", x11_id, flags);

                if (flags & XISlaveRemoved) {
                    RB_TRACE("removing slave device");
                    remove_device(x11_id);
                } else if (flags & XISlaveAdded) {
                    RB_TRACE("querying new slave device");
                    x11_device_query query{m_display.get(), x11_id};

                    if (query.begin() == nullptr) {
                        RB_TRACE("failed to query the device, skipping");
                        continue;
                    }

                    add_device(*query.begin());
                }

                RB_TRACE("check if device exists to set enabled status");
                auto device_ptr = get_device(x11_id);
                if (device_ptr == nullptr) {
                    continue;
                }

                RB_TRACE("setting enabled status");
                device_ptr->set_usable(info.enabled);
            }
        }
    }
}
