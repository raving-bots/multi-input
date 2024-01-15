// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string>

#include "utils.hpp"
#include "api_types.hpp"
#include "log_level.hpp"
#include "device.hpp"
#include "source.hpp"
#include "format.hpp"

namespace multi_input {
    struct options {
        using log_callback    = std::function<void(log_level, api_string)>;
        using device_callback = std::function<void(device_event, device_id, api_device*)>;

        RB_COPYABLE(options);

        options();

        void set_log_level(log_level);
        void set_null_log_sink();
        void set_stderr_log_sink();
        void set_custom_log_sink(log_callback);
        void set_null_device_callback();
        void set_custom_device_callback(device_callback);
    private:
        friend struct context;

        log_callback m_log_sink;
        device_callback m_device_callback;
        log_level m_log_level;
    };

    struct context {
        using find_callback = std::function<bool(device_id, input_code, float, float, float)>;

        RB_MOVEABLE(context);

        explicit context(options);

        void set_options(const options&);
        const options& get_options() const;

        void drain_events();
        void reset();

        template <typename... Args>
        void log_verbose(const std::string& fmt, Args&&... args) {
#ifdef RB_DEBUG
            log_args(log_level::debug_verbose, fmt, std::forward<Args>(args)...);
#endif
        }

        template <typename... Args>
        void log_debug(const std::string& fmt, Args&&... args) {
            log_args(log_level::debug, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void log_info(const std::string& fmt, Args&&... args) {
            log_args(log_level::info, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void log_warning(const std::string& fmt, Args&&... args) {
            log_args(log_level::warning, fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void log_error(const std::string& fmt, Args&&... args) {
            log_args(log_level::error, fmt, std::forward<Args>(args)...);
        }

        void log(log_level, const std::string&);
        void log_exception();

        device* get_device(device_id);
        std::vector<device*> get_devices();

        device_id get_next_id();
        void add_device(std::unique_ptr<device>);
        void remove_device(device_id);
        void notify_device(device_id, device_event);

        bool find_first(find_callback, input_code*, input_code*, device_id*, input_code*);
    private:
        template <typename T>
        void add_source() {
            RB_TRACE_ENTER();

            try {
                auto source = std::make_unique<T>(this);
                source->enum_devices();
                m_sources.emplace_back(std::move(source));
            } catch (...) {
                log_exception();
            }
        }

        template <typename... Args>
        void log_args(log_level level, const std::string& fmt_str, Args&&... args) {
            log(level, format(fmt_str, std::forward<Args>(args)...));
        }

        options m_options;
        std::vector<std::unique_ptr<source>> m_sources;
        std::unordered_map<device_id, std::unique_ptr<device>> m_devices;
        device_id m_next_unique_id;
    };
}
