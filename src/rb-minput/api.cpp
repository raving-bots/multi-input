// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <exception>
#include <functional>
#include <algorithm>

#include "api.hpp"
#include "context.hpp"
#include "enumeration.hpp"
#include "virtual_axis.hpp"
#include "log_level.hpp"
#include "utils.hpp"

// TODO figure out good error API

namespace multi_input {
#ifdef RB_ENABLE_TRACING
#   define RB_GUARD_ARGS RB_TRACE_OBJECT, ctx,
    template <typename T, typename Fn>
    auto with_guard(debug::tracer& tracer, context* ctx, Fn&& fn) -> T {
        if (ctx == nullptr) {
            tracer.output("nullptr context");
            return T{};
        }

        try {
            return fn();
        } catch (...) {
            tracer.output("exception");
            try {
                tracer.output("logging exception");
                ctx->log_exception();
            } catch (...) {
                tracer.output("failed to log exception");
            }
            return T{};
        }
    }
#else // RB_ENABLE_TRACING
#   define RB_GUARD_ARGS ctx,
    template <typename T, typename Fn>
    auto with_guard(context* ctx, Fn&& fn) -> T {
        if (ctx == nullptr) {
            return T{};
        }

        try {
            return fn();
        } catch (...) {
            try {
                ctx->log_exception();
            } catch (...) {
            }
            return T{};
        }
    }
#endif

    // basic api
    RB_API options* RB_APICALL_POST rb_minput_create_options() {
        RB_TRACE_ENTER();

        try {
            RB_TRACE("creating options");
            return new options();
        } catch (...) {
            RB_TRACE("exception");
            return nullptr;
        }
    }

    RB_API api_bool RB_APICALL_POST rb_minput_set_log_level(options* opts, log_level level) {
        RB_TRACE_ENTER();

        if (opts == nullptr) {
            RB_TRACE("nullptr opts");
            return 0;
        }

        try {
            RB_TRACE("setting log level");
            opts->set_log_level(level);
            return 1;
        } catch (...) {
            RB_TRACE("exception");
            return 0;
        }
    }

    RB_API api_bool RB_APICALL_POST rb_minput_set_stderr_log_sink(options* opts) {
        RB_TRACE_ENTER();

        if (opts == nullptr) {
            RB_TRACE("nullptr opts");
            return 0;
        }

        try {
            RB_TRACE("setting stderr log sink");
            opts->set_stderr_log_sink();
            return 1;
        } catch (...) {
            RB_TRACE("exception");
            return 0;
        }
    }

    RB_API api_bool RB_APICALL_POST rb_minput_set_custom_log_sink(options* opts, log_callback callback, user_data data) {
        RB_TRACE_ENTER();

        if (opts == nullptr) {
            RB_TRACE("nullptr opts");
            return 0;
        }

        try {
            if (callback == nullptr) {
                RB_TRACE("setting null log sink");
                opts->set_null_log_sink();
            } else {
                RB_TRACE("setting custom log sink");
                using namespace std::placeholders;
                opts->set_custom_log_sink(std::bind(callback, data, _1, _2));
            }
            return 1;
        } catch (...) {
            RB_TRACE("exception");
            return 0;
        }
    }

    RB_API api_bool RB_APICALL_POST rb_minput_set_device_callback(options* opts, device_callback callback, user_data data) {
        RB_TRACE_ENTER();

        if (opts == nullptr) {
            RB_TRACE("nullptr opts");
            return 0;
        }

        try {
            if (callback == nullptr) {
                RB_TRACE("setting null device callback");
                opts->set_null_device_callback();
            } else {
                RB_TRACE("setting custom device callback");
                using namespace std::placeholders;
                opts->set_custom_device_callback(std::bind(callback, data, _1, _2, _3));
            }
            return 1;
        } catch (...) {
            RB_TRACE("exception");
            return 0;
        }
    }

    RB_API api_bool RB_APICALL_POST rb_minput_destroy_options(options* opts) {
        RB_TRACE_ENTER();

        if (opts == nullptr) {
            RB_TRACE("nullptr opts");
            return 0;
        }

        try {
            RB_TRACE("destroying options");
            delete opts;
            return 1;
        } catch (...) {
            RB_TRACE("exception");
            return 0;
        }
    }

    RB_API context* RB_APICALL_POST rb_minput_create(options* opts) {
        RB_TRACE_ENTER();

        if (opts == nullptr) {
            RB_TRACE("nullptr opts");
            return nullptr;
        }

        try {
            RB_TRACE("creating the context");
            return new context(*opts);
        } catch (...) {
            RB_TRACE("exception");
            return nullptr;
        }
    }

    RB_API options* RB_APICALL_POST rb_minput_get_options(context* ctx) {
        RB_TRACE_ENTER();

        return with_guard<options*>(RB_GUARD_ARGS [&](){
            RB_TRACE("copying current options");
            return new options(ctx->get_options());
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_set_options(context* ctx, options *opts) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            if (opts == nullptr) {
                RB_TRACE("nullptr opts");
                return 0;
            }

            RB_TRACE("setting new options");
            ctx->set_options(*opts);
            return 1;
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_destroy(context* ctx) {
        RB_TRACE_ENTER();

        if (ctx == nullptr) {
            RB_TRACE("nullptr context");
            return 0;
        }

        try {
            RB_TRACE("destroying the context");
            delete ctx;
            return 1;
        } catch (...) {
            RB_TRACE("exception");
            return 0;
        }
    }

    RB_API api_bool RB_APICALL_POST rb_minput_reset(context* ctx) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("resetting all axes");
            ctx->reset();
            return 1;
        });
    }

    // events
    RB_API api_bool RB_APICALL_POST rb_minput_drain_events(context* ctx) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("draining events");
            ctx->drain_events();
            return 1;
        });
    }

    // device list
    RB_API enumeration* RB_APICALL_POST rb_minput_get_devices(context* ctx) {
        RB_TRACE_ENTER();

        return with_guard<enumeration*>(RB_GUARD_ARGS [&](){
            RB_TRACE("creating enumeration");
            return new enumeration(ctx);
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_next_device(context* ctx, enumeration* enum_, api_device* buffer) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            if (enum_ == nullptr) {
                RB_TRACE("nullptr enum_");
                ctx->log_error(u8"next_device: enumeration handle must not be NULL");
                return 0;
            }

            if (buffer == nullptr) {
                RB_TRACE("nullptr buffer");
                ctx->log_error(u8"next_device: buffer must not be NULL");
                return 0;
            }

            RB_TRACE("grabbing device from iterator");
            auto device = enum_->next();

            if (device == nullptr) {
                RB_TRACE("iterator exhausted");
                return 0;
            } else {
                RB_TRACE("filling the buffer");
                buffer->set_from(*device);
                return 1;
            }
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_destroy_enumeration(context* ctx, enumeration* enum_) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            if (enum_ == nullptr) {
                RB_TRACE("nullptr enum_");
                ctx->log_error(u8"destroy_enumeration: enumeration handle must not be NULL");
                return 0;
            }

            RB_TRACE("destroying enumeration");
            delete enum_;
            return 1;
        });
    }

    // device
    RB_API api_bool RB_APICALL_POST rb_minput_get_device(context* ctx, device_id id, api_device* buffer) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            if (buffer == nullptr) {
                RB_TRACE("nullptr buffer");
                ctx->log_error(u8"get_device: buffer must not be NULL");
                return 0;
            }

            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                return 0;
            } else {
                RB_TRACE("filling the buffer");
                buffer->set_from(*device);
                return 1;
            }
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_is_usable(context* ctx, device_id id) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                return 0;
            } else {
                return device->is_usable() ? 1 : 0;
            }
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_can_vibrate(context* ctx, device_id id) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                return 0;
            } else {
                return device->can_vibrate() ? 1 : 0;
            }
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_vibrate(context* ctx, device_id id, int duration, float left, float right) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            if (duration < 0 || duration > 32767) {
                RB_TRACE("duration out of range");
                ctx->log_error(u8"vibrate: duration must be an integer value between 0 and 32767 (got %1%)", duration);
                return 0;
            }

            if (left < 0.f || left > 1.f) {
                RB_TRACE("left out of range");
                ctx->log_error(u8"vibrate: left motor strength must be a float value between 0 and 1 (got %1%)", left);
                return 0;
            }

            if (right < 0.f || right > 1.f) {
                RB_TRACE("right out of range");
                ctx->log_error(u8"vibrate: right motor strength must be a float value between 0 and 1 (got %1%)", right);
                return 0;
            }

            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"vibrate: device %1% not found", id);
                return 0;
            } else {
                RB_TRACE("sending vibrate request");
                return device->vibrate(duration, left, right) ? 1 : 0;
            }
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_reset_device(context* ctx, device_id id) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"reset_device: device %1% not found", id);
                return 0;
            } else {
                RB_TRACE("resetting device");
                device->reset();
                return 1;
            }
        });
    }

    RB_API size_t RB_APICALL_POST rb_minput_get_axis_count(context* ctx, device_id id) {
        RB_TRACE_ENTER();

        return with_guard<size_t>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"get_axis_count: device %1% not found", id);
                return size_t{};
            } else {
                RB_TRACE("returning axis count");
                return device->get_axis_count();
            }
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_get_axes(context* ctx, device_id id, input_code* buffer, size_t buffer_size) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"get_axes: device %1% not found", id);
                return 0;
            }

            if (buffer == nullptr) {
                RB_TRACE("nullptr buffer");
                ctx->log_error(u8"get_axes: buffer set to nullptr");
                return 0;
            }

            if (buffer_size == 0) {
                RB_TRACE("zero-size buffer");
                ctx->log_error(u8"get_axes: buffer_size set to 0");
                return 0;
            }

            RB_TRACE("copying supported axes");
            auto axes = device->get_axis_codes();

            buffer_size = std::min(buffer_size, axes.size());

            auto it = std::begin(axes);
            std::copy(it, it + buffer_size, buffer);
            return 1;
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_has_axis(context* ctx, device_id id, input_code code) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"get_supported_axes: device %1% not found", id);
                return 0;
            }

            return device->get_axis(code) != nullptr ? 1 : 0;
        });
    }
    // virtual axes
    RB_API api_bool RB_APICALL_POST rb_minput_get_values(context* ctx, device_id id, input_code code, float* current, float* previous, float* next) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"get_values: device %1% not found", id);
                return 0;
            }

            RB_TRACE("grabbing axis");
            auto axis = device->get_axis(code);

            if (axis == nullptr) {
                RB_TRACE("axis not found");
                return 0;
            }

            if (current != nullptr) {
                *current = axis->get();
            }

            if (previous != nullptr) {
                *previous = axis->get_previous();
            }

            if (next != nullptr) {
                *next = axis->get_next();
            }

            return 1;
        });
    }

    RB_API float RB_APICALL_POST rb_minput_get_value(context* ctx, device_id id, input_code code) {
        RB_TRACE_ENTER();

        return with_guard<float>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"get_values: device %1% not found", id);
                return 0.0f;
            }

            RB_TRACE("grabbing axis");
            auto axis = device->get_axis(code);

            if (axis == nullptr) {
                RB_TRACE("axis not found");
                return 0.0f;
            }

            return axis->get();
        });
    }

    RB_API float RB_APICALL_POST rb_minput_get_previous(context* ctx, device_id id, input_code code) {
        RB_TRACE_ENTER();

        return with_guard<float>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"get_values: device %1% not found", id);
                return 0.0f;
            }

            RB_TRACE("grabbing axis");
            auto axis = device->get_axis(code);

            if (axis == nullptr) {
                RB_TRACE("axis not found");
                return 0.0f;
            }

            return axis->get_previous();
        });
    }

    RB_API float RB_APICALL_POST rb_minput_get_next(context* ctx, device_id id, input_code code) {
        RB_TRACE_ENTER();

        return with_guard<float>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"get_values: device %1% not found", id);
                return 0.0f;
            }

            RB_TRACE("grabbing axis");
            auto axis = device->get_axis(code);

            if (axis == nullptr) {
                RB_TRACE("axis not found");
                return 0.0f;
            }

            return axis->get_next();
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_set_value(context* ctx, device_id id, input_code code, float value) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"set_value: device %1% not found", id);
                return 0;
            }

            RB_TRACE("grabbing axis");
            auto axis = device->get_axis(code);

            if (axis == nullptr) {
                RB_TRACE("axis not found");
                ctx->log_warning(u8"set_value: axis %1% not found on device %2%", static_cast<int>(code), id);
                return 0;
            }

            axis->set(value);
            return 1;
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_add_value(context* ctx, device_id id, input_code code, float value) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"add_value: device %1% not found", id);
                return 0;
            }

            RB_TRACE("grabbing axis");
            auto axis = device->get_axis(code);

            if (axis == nullptr) {
                RB_TRACE("axis not found");
                ctx->log_warning(u8"add_value: axis %1% not found on device %2%", static_cast<int>(code), id);
                return 0;
            }

            axis->add(value);
            return 1;
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_commit_value(context* ctx, device_id id, input_code code) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            RB_TRACE("grabbing device");
            auto device = ctx->get_device(id);

            if (device == nullptr) {
                RB_TRACE("device not found");
                ctx->log_warning(u8"commit_value: device %1% not found", id);
                return 0;
            }

            RB_TRACE("grabbing axis");
            auto axis = device->get_axis(code);

            if (axis == nullptr) {
                RB_TRACE("axis not found");
                ctx->log_warning(u8"commit_value: axis %1% not found on device %2%", static_cast<int>(code), id);
                return 0;
            }

            axis->commit();
            return 1;
        });
    }

    RB_API api_bool RB_APICALL_POST rb_minput_find_first(context* ctx, find_callback callback, user_data data, input_code* in_codes, size_t in_size, device_id* out_id, input_code* out_code) {
        RB_TRACE_ENTER();

        return with_guard<api_bool>(RB_GUARD_ARGS [&](){
            auto cb = [&](device_id id, input_code code, float current, float previous, float next) {
                return callback(data, id, code, current, previous, next) != 0;
            };

            input_code *begin, *end;
            if (in_codes == nullptr || in_size == 0) {
                RB_TRACE("not using in_codes");
                begin = end = nullptr;
            } else {
                RB_TRACE("using in_codes");
                begin = in_codes;
                end   = in_codes + in_size;
            }

            RB_TRACE("calling find_first");
            return ctx->find_first(cb, begin, end, out_id, out_code);
        });
    }
}
