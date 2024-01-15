// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <string>
#include <stdexcept>
#include <utility>

#include "format.hpp"

#include <IOKit/IOReturn.h>

namespace multi_input {
    namespace osx {
        const char* get_ioreturn_string(IOReturn value);

        template <typename... Args>
        void throw_osx_error(const std::string& fmt, Args&&... args) {
            auto message = format(fmt, std::forward<Args>(args)...);
            throw std::runtime_error(message);
        }

        template <typename... Args>
        void throw_osx_error(IOReturn code, const std::string& fmt, Args&&... args) {
            auto message = format(fmt, std::forward<Args>(args)...);
            auto code_message = get_ioreturn_string(code);

            if (code_message != nullptr) {
                message = format("%1%: %2%", message, code_message);
            } else {
                message = format("%|s|: unknown error code %|x|", message, code);
            }

            throw std::runtime_error(message);
        }
    }
}
