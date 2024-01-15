// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#define RB_USE_LIB

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>

#include <boost/algorithm/string/predicate.hpp>

#if defined(RB_PLATFORM_WINDOWS)
#   include <windows.h>
#elif defined(RB_PLATFORM_OSX)
#   include <CoreFoundation/CoreFoundation.h>
#elif defined(RB_PLATFORM_LINUX)
#   include "linux/posix.hpp"
#endif

#include "api.hpp"
#include "device.hpp"
#include "enums.hpp"

using namespace multi_input;

#define ensure(x) ([&](){ if(!(x)) { fail(#x); } })()

void fail(const char *msg) {
    throw std::runtime_error(msg);
}

RB_APICALL_PRE int RB_APICALL_POST any_non_zero(user_data, device_id id, input_code code, float current, float previous, float next) {
    return current != 0 ? 1 : 0;
}

#if defined(RB_PLATFORM_WINDOWS)
void drain_system_events() {
    MSG msg{};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        //std::cout << "MSG: " << static_cast<void*>(msg.hwnd) << " = " << msg.message << "\n";
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void sleep_ms(int ms) {
    Sleep(ms);
}
#elif defined(RB_PLATFORM_OSX)
void drain_system_events() {
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, TRUE);
}

void sleep_ms(int ms) {
    usleep(ms * 1000);
}
#else
void drain_system_events() {
}

void sleep_ms(int ms) {
    usleep(ms * 1000);
}
#endif

std::string g_rumble_name;

template <typename Fn>
void enumerate_devices(context *ctx, Fn&& fn) {
    auto enum_ = rb_minput_get_devices(ctx);
    ensure(enum_);

    api_device info{};
    while (rb_minput_next_device(ctx, enum_, &info)) {
        fn(ctx, info);
    }

    ensure(rb_minput_destroy_enumeration(ctx, enum_));
}

std::vector<input_code> get_axes(context *ctx, device_id id) {
    std::vector<input_code> axes{};
    auto count = rb_minput_get_axis_count(ctx, id);

    if (count > 0) {
        axes.resize(count);
        ensure(rb_minput_get_axes(ctx, id, &axes[0], count));
    }

    return axes;
}

void print_info(context *ctx, api_device& info) {
    std::cout << "**** Device #" << info.m_id << ":\n";
    std::cout << "\tAxis count:  " << info.m_axis_count << "\n";
    std::cout << "\tUsable:      " << (info.m_is_usable ? "yes" : "no") << "\n";
    std::cout << "\tCan vibrate: " << (info.m_can_vibrate ? "yes" : "no") << "\n";
    std::cout << "\tName:        " << info.m_name << "\n";
    std::cout << "\tLocation:    " << info.m_location_id << "\n";

    if (*info.m_internal_id) {
        std::cout << "\tInternal ID: " << info.m_internal_id << "\n";
    }
    if (info.m_vendor_id) {
        std::cout << "\tVendor ID:   " << std::hex << info.m_vendor_id << std::dec << "\n";
    }
    if (info.m_product_id) {
        std::cout << "\tProduct ID:  " << std::hex << info.m_product_id << std::dec << "\n";
    }
    if (info.m_revision) {
        std::cout << "\tRevision:    " << std::hex << info.m_revision << std::dec << "\n";
    }
    if (*info.m_serial) {
        std::cout << "\tSerial:      " << info.m_serial << "\n";
    }

    if (ctx != nullptr) {
        //for (auto&& axis : get_axes(ctx, info.m_id)) {
        //    std::cout << "\t" << to_string(axis) << "\n" << std::flush;
        //}

        std::string name = info.m_name;
        if (!g_rumble_name.empty() && boost::icontains(name, g_rumble_name)) {
            std::cout << "!! Trying to rumble " << g_rumble_name << "\n" << std::flush;
            ensure(rb_minput_vibrate(ctx, info.m_id, 1000, 1.0f, 0.0f));
        }
    }

    std::cout << "\n" << std::flush;
}

RB_APICALL_PRE void RB_APICALL_POST on_device_event(user_data, device_event event, device_id id, api_device *info) {
    std::cout << "Device #" << id << " event = " << to_string(event) << "\n";
    if (info != nullptr) {
        print_info(nullptr, *info);
    }
}

int main(int argc, char **argv) {
    std::vector<std::string> args{};
    auto no_loop = false;

    for (auto idx = 1; idx < argc; ++idx) {
        args.emplace_back(argv[idx]);
    }

    for (auto it = args.begin(); it != args.end(); ++it) {
        if (*it == "--rumble") {
            if (++it == args.end()) {
                std::cout << "** Error: --rumble requires an argument\n";
                return 1;
            }

            g_rumble_name = *it;
        } else if (*it == "--no-loop") {
            no_loop = true;
        }
    }

    auto opts = rb_minput_create_options();
    ensure(opts);
    ensure(rb_minput_set_log_level(opts, log_level::debug_verbose));
    ensure(rb_minput_set_stderr_log_sink(opts));
    ensure(rb_minput_set_device_callback(opts, on_device_event, nullptr));

    auto ctx = rb_minput_create(opts);
    ensure(ctx);
    ensure(rb_minput_destroy_options(opts));

    std::cout << "==[ Initial enumeration ]==\n" << std::flush;
    enumerate_devices(ctx, print_info);

    if (no_loop) {
        return 0;
    }

    while (true) {
        drain_system_events();
        ensure(rb_minput_drain_events(ctx));

        device_id first_id;
        input_code first_axis{};

        if (rb_minput_find_first(ctx, any_non_zero, nullptr, nullptr, 0, &first_id, &first_axis)) {
            std::cout << "===========================\n";
            std::cout << "** find_first: device #" << first_id << " axis " << to_string(first_axis) << "\n\n" << std::flush;
            enumerate_devices(ctx, [&](context*, api_device& info) {
                ensure(rb_minput_get_axis_count(ctx, info.m_id) == info.m_axis_count);

                if (info.m_axis_count == 0 || !info.m_is_usable) {
                    return;
                }

                for (auto&& axis : get_axes(ctx, info.m_id)) {
                    float current, previous, next;
                    ensure(rb_minput_get_values(ctx, info.m_id, axis, &current, &previous, &next));

                    if (current == 0) {
                        continue;
                    }

                    std::cout << "\t" << to_string(axis) << " = " << current << " (prev = " << previous << ", next = " << next << ")\n" << std::flush;
                }
            });
        }

        sleep_ms(10);
    }
}
