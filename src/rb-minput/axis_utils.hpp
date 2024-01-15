// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <cmath>

namespace multi_input {
    struct device;

    constexpr float deadzone_left_stick  = 7849.0f;
    constexpr float deadzone_right_stick = 8689.0f;
    constexpr float deadzone_trigger     = 30.0f;

    void derive_mouse_pre_commit(device&);
    void derive_mouse_post_commit(device&);

    inline float apply_deadzone(float value, float abs_max, float deadzone) {
        if (value <= deadzone) {
            return 0;
        } else {
            if (value > abs_max) {
                value = abs_max;
            }

            value -= deadzone;
            return value / (abs_max - deadzone);
        }
    }

    void derive_stick_pre_commit(device&);
}
