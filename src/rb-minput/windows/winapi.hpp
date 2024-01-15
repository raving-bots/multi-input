// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <memory>
#include <stdexcept>

#include <windows.h>
#include <xinput.h>
#include <setupapi.h>
#include <devpropdef.h>
#include <devpkey.h>

#include "format.hpp"

namespace multi_input {
    namespace windows {
        std::string to_utf8(const std::wstring&);
        std::string describe_winapi_error(DWORD error_code);
        std::string describe_winapi_error();

        template <typename... Args>
        std::string format_winapi_error(DWORD error_code, const std::string& fmt, Args&&... args) {
            auto error_message = describe_winapi_error(error_code);
            auto message = format(fmt, std::forward<Args>(args)...);
            auto full_message = format("%1%: %2%", message, error_message);
            return full_message;
        }

        template <typename... Args>
        std::string format_winapi_error(const std::string& fmt, Args&&... args) {
            return format_winapi_error(GetLastError(), fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        void throw_winapi_error(DWORD error_code, const std::string& fmt, Args&&... args) {
            auto full_message = format_winapi_error(error_code, fmt, std::forward<Args>(args)...);
            throw std::runtime_error(full_message);
        }

        template <typename... Args>
        void throw_winapi_error(const std::string& fmt, Args&&... args) {
            throw_winapi_error(GetLastError(), fmt, std::forward<Args>(args)...);
        }
    }
}
