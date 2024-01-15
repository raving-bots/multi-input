// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "input_code.hpp"
#include "device_event.hpp"
#include "log_level.hpp"

namespace multi_input {
    inline std::string to_string(device_event value) {
        switch (value) {
            default: return "unknown";
            case device_event::created: return "created";
            case device_event::removed: return "removed";
            case device_event::unusable: return "unusable";
            case device_event::usable: return "usable";
        }
    }
}
