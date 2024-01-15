// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <cstdint>
#include <type_traits>
#include <string>

#ifdef RB_CPU_X86
#   ifdef _MSC_VER
#       define RB_APICALL __stdcall
#       define RB_APICALL_PRE
#       define RB_APICALL_POST RB_APICALL
#   else
#       define RB_APICALL __attribute__((stdcall))
#       define RB_APICALL_PRE RB_APICALL
#       define RB_APICALL_POST
#   endif
#else
#   define RB_APICALL
#   define RB_APICALL_PRE
#   define RB_APICALL_POST
#endif

#if defined(RB_PLATFORM_WINDOWS)
#   ifdef RB_USE_LIB
#       define RB_DLL_SPEC dllimport
#   else
#       define RB_DLL_SPEC dllexport
#   endif

#   ifdef _MSC_VER
#       define RB_API extern "C" __declspec(RB_DLL_SPEC) RB_APICALL_PRE
#   else
#       define RB_API extern "C" __attribute__((RB_DLL_SPEC)) RB_APICALL_PRE
#   endif
#elif defined(RB_PLATFORM_LINUX) or defined(RB_PLATFORM_OSX)
#   define RB_API extern "C" __attribute__((visibility("default"))) RB_APICALL_PRE
#else
#   error Platform not supported yet.
#endif

namespace multi_input {
    struct context;
    struct options;
    struct enumeration;
    struct api_device;
    enum class log_level;
    enum class input_code;
    enum class device_event;

    using api_string = const char*;
    using user_data = void*;
    using api_bool = int32_t;
    using api_int = int32_t;
    using api_float = float;
    using device_id = int64_t;
    using log_callback = void(RB_APICALL *)(user_data, log_level, api_string /* message */);
    using find_callback = api_bool(RB_APICALL *)(user_data, device_id, input_code, api_float, api_float, api_float);
    using device_callback = void(RB_APICALL *)(user_data, device_event, device_id, api_device*);
}
