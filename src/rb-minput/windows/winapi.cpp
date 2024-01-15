// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <string>

#include "windows/winapi.hpp"

namespace multi_input {
    namespace windows {
        std::string to_utf8(const std::wstring& ws) {
            if (ws.empty()) {
                return "";
            }

            std::string buffer{};

            auto size = WideCharToMultiByte(CP_UTF8, 0, &ws[0], ws.size(), nullptr, 0, nullptr, nullptr);
            if (size == 0) {
                throw_winapi_error("Failed to get the size for UTF-8 buffer");
            }

            buffer.resize(size);

            size = WideCharToMultiByte(CP_UTF8, 0, &ws[0], ws.size(), &buffer[0], size, nullptr, nullptr);
            if (size == 0) {
                throw_winapi_error("Failed to recode string to UTF-8");
            }

            return buffer;
        }

        std::string describe_winapi_error(DWORD error_code) {
            char *raw_ptr;

            auto flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS;
            auto size = FormatMessageA(
                flags,
                nullptr,
                error_code,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<char*>(&raw_ptr),
                0,
                nullptr);

            std::string error_message;
            if (size == 0) {
                error_message = format("error code %|x| (FormatMessage failed with error code %|x|)", error_code, GetLastError());
            } else {
                std::unique_ptr<char, decltype(&LocalFree)> ptr(raw_ptr, LocalFree);
                error_message = std::string(raw_ptr, raw_ptr + size - 2);
            }

            return error_message;
        }

        std::string describe_winapi_error() {
            return describe_winapi_error(GetLastError());
        }
    }
}
