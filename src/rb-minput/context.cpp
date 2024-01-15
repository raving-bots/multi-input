// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <iostream>
#include <exception>
#include <memory>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/range/iterator_range_core.hpp>

#include "context.hpp"
#include "log_level.hpp"
#include "input_code.hpp"
#include "device.hpp"
#include "source.hpp"
#include "device_event.hpp"

#if defined(RB_PLATFORM_LINUX)
#   include "linux/xi2/xi2_source.hpp"
#   include "linux/evdev/evdev_source.hpp"
#elif defined(RB_PLATFORM_WINDOWS)
#   include "windows/raw_input/raw_input_source.hpp"
#   include "windows/xinput/xinput_source.hpp"
#elif defined(RB_PLATFORM_OSX)
#   include "osx/hidm/hidm_source.hpp"
#else
#   error Add platform includes.
#endif

namespace multi_input {
    void stderr_log_sink(log_level level, api_string message) {
        std::cerr << u8"multi-input: " << to_string(level) << u8": " << message << u8"\n" << std::flush;
    }

    void null_log_sink(log_level, api_string) {
    }

    void null_device_callback(device_event, device_id, api_device*) {
    }

    options::options()
        : m_log_sink(null_log_sink), m_device_callback(null_device_callback), m_log_level(log_level::info)
    {
    }

    void options::set_log_level(log_level level) {
        m_log_level = level;
    }

    void options::set_stderr_log_sink() {
        m_log_sink = stderr_log_sink;
    }

    void options::set_null_log_sink() {
        m_log_sink = null_log_sink;
    }

    void options::set_custom_log_sink(options::log_callback sink) {
        m_log_sink = sink;
    }

    void options::set_null_device_callback() {
        m_device_callback = null_device_callback;
    }

    void options::set_custom_device_callback(device_callback callback) {
        m_device_callback = callback;
    }

    context::context(options opts)
        : m_options(opts), m_sources(), m_devices(), m_next_unique_id(1)
    {
        RB_TRACE_ENTER();

#if defined(RB_PLATFORM_LINUX)
        RB_TRACE("adding XI2 source");
        log_debug("Adding source: X11 XInput2");
        add_source<lnx::xi2_source>();

        RB_TRACE("adding evdev source");
        log_debug("Adding source: evdev");
        add_source<lnx::evdev_source>();
#elif defined(RB_PLATFORM_WINDOWS)
        RB_TRACE("adding Raw Input source");
        log_debug("Adding source: Raw Input");
        add_source<windows::raw_input_source>();

        RB_TRACE("adding XInput source");
        log_debug("Adding source: XInput");
        add_source<windows::xinput_source>();
#elif defined(RB_PLATFORM_OSX)
        RB_TRACE("adding HIDManager source");
        log_debug("Adding source: HIDManager");
        add_source<osx::hidm_source>();
#else
#   error Add platform sources.
#endif
    }

    void context::set_options(const options& opts) {
        m_options = opts;
    }

    const options& context::get_options() const {
        return m_options;
    }

    void context::drain_events() {
        for (auto&& source : m_sources) {
            source->drain_events();
        }

        for (auto&& pair : m_devices) {
            pair.second->commit();
        }
    }

    void context::reset() {
        for (auto&& pair : m_devices) {
            pair.second->reset();
        }
    }

    void context::log(log_level level, const std::string& message) {
        if (static_cast<int>(level) >= static_cast<int>(m_options.m_log_level)) {
            m_options.m_log_sink(level, message.c_str());
        }
    }

    void context::log_exception() {
        auto ptr = std::current_exception();
        if (ptr == nullptr) {
            return;
        }

        auto info = boost::current_exception_diagnostic_information();
        log_error(u8"Native exception caught: %1%", info);
    }

    device* context::get_device(device_id id) {
        auto it = m_devices.find(id);
        if (it == m_devices.end()) {
            return nullptr;
        } else {
            return it->second.get();
        }
    }

    std::vector<device*> context::get_devices() {
        std::vector<device*> buffer{};
        buffer.reserve(m_devices.size());

        for (auto&& pair : m_devices) {
            buffer.emplace_back(pair.second.get());
        }

        return buffer;
    }

    device_id context::get_next_id() {
        return m_next_unique_id++;
    }

    void context::add_device(std::unique_ptr<device> dev) {
        auto id = dev->get_id();
        log_debug(u8"Adding device %s (%s)", id, dev->get_name());
        m_devices.emplace(id, std::move(dev));

        notify_device(id, device_event::created);
    }

    void context::remove_device(device_id id) {
        auto it = m_devices.find(id);
        if (it == m_devices.end()) {
            return;
        }

        log_debug(u8"Removing device %1% (%2%)", id, it->second->get_name());
        m_devices.erase(it);

        notify_device(id, device_event::removed);
    }

    void context::notify_device(device_id id, device_event event) {
        auto it = m_devices.find(id);
        if (it == m_devices.end()) {
            m_options.m_device_callback(event, id, nullptr);
        } else {
            api_device api_dev{};
            api_dev.set_from(*it->second);
            m_options.m_device_callback(event, id, &api_dev);
        }
    }

    bool find_first_device(context::find_callback callback, device& dev, input_code* it, input_code* end, device_id* out_id, input_code* out_code) {
        while (it != end) {
            auto code = *it;
            auto axis = dev.get_axis(code);

            ++it;
            if (axis == nullptr) {
                continue;
            }

            auto id = dev.get_id();

            if (callback(id, code, axis->get(), axis->get_previous(), axis->get_next())) {
                if (out_code != nullptr) {
                    *out_code = code;
                }

                if (out_id != nullptr) {
                    *out_id = id;
                }

                return true;
            }
        }

        return false;
    }

    bool context::find_first(context::find_callback callback, input_code* begin, input_code* end, device_id* out_id, input_code* out_code) {
        for (auto&& pair : m_devices) {
            auto&& dev = *pair.second;

            if (!dev.is_usable()) {
                continue;
            }

            if (begin == nullptr || end == nullptr) {
                auto supported = dev.get_axis_codes();
                if (supported.size() == 0) {
                    continue;
                }

                auto it = &supported[0];
                if (find_first_device(callback, dev, it, it + supported.size(), out_id, out_code)) {
                    return true;
                }
            } else if (find_first_device(callback, dev, begin, end, out_id, out_code)) {
                return true;
            }
        }

        return false;
    }
}
