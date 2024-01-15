// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "utils.hpp"
#include "api_types.hpp"

namespace multi_input {
    enum class log_level : int {
        debug_verbose = 0,
        debug = 1,
        info = 2,
        warning = 3,
        error = 4
    };

    inline api_string to_string(log_level level) {
        switch (level) {
            case log_level::debug_verbose:
                return u8"verbose";
            case log_level::debug:
                return u8"debug";
            case log_level::info:
                return u8"info";
            case log_level::warning:
                return u8"warning";
            case log_level::error:
                return u8"error";
            default:
                return u8"unknown";
        }
    }
}
