// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "axis_utils.hpp"
#include "device.hpp"
#include "virtual_axis.hpp"
#include "input_code.hpp"

namespace multi_input {
    namespace detail {
        void derive_axis(device& dev, input_code source, input_code negative, input_code positive) {
            auto axis = dev.get_axis(source);
            auto negative_axis = dev.get_axis(negative);
            auto positive_axis = dev.get_axis(positive);

            if (axis == nullptr || negative_axis == nullptr || positive_axis == nullptr) {
                return;
            }

            auto value = axis->get_next();

            if (value > 0) {
                positive_axis->set(value);
                negative_axis->set(0);
            } else {
                positive_axis->set(0);
                negative_axis->set(-value);
            }
        }

        void derive_button_axis(device& dev, input_code negative, input_code positive, input_code target) {
            auto axis = dev.get_axis(target);
            auto negative_axis = dev.get_axis(negative);
            auto positive_axis = dev.get_axis(positive);

            if (axis == nullptr || negative_axis == nullptr || positive_axis == nullptr) {
                return;
            }

            auto negative_value = negative_axis->get_next();
            auto positive_value = positive_axis->get_next();

            if (negative_value != 0) {
                axis->set(-negative_value);
            } else {
                axis->set(positive_value);
            }
        }

        void maybe_reset_axis(device& dev, input_code code) {
            auto axis = dev.get_axis(code);

            if (axis == nullptr) {
                return;
            }

            // in case the platform doesn't zero the relative axes
            // TODO absolute axis support
            axis->set(0);
        }
    }

    void derive_mouse_pre_commit(device& dev) {
        // reverse Y axis
        auto axis = dev.get_axis(input_code::mouse_y);
        if (axis != nullptr) {
            axis->set(-axis->get_next());
        }

        detail::derive_axis(dev, input_code::mouse_x, input_code::mouse_x_left, input_code::mouse_x_right);
        detail::derive_axis(dev, input_code::mouse_y, input_code::mouse_y_down, input_code::mouse_y_up);
        detail::derive_axis(dev, input_code::mouse_wheel, input_code::mouse_wheel_down, input_code::mouse_wheel_up);
    }

    void derive_mouse_post_commit(device& dev) {
        detail::maybe_reset_axis(dev, input_code::mouse_x);
        detail::maybe_reset_axis(dev, input_code::mouse_y);
        detail::maybe_reset_axis(dev, input_code::mouse_wheel);
    }

    void derive_stick_pre_commit(device& dev) {
        detail::derive_axis(dev, input_code::pad_left_stick_x, input_code::pad_left_stick_left, input_code::pad_left_stick_right);
        detail::derive_axis(dev, input_code::pad_left_stick_y, input_code::pad_left_stick_down, input_code::pad_left_stick_up);
        detail::derive_axis(dev, input_code::pad_right_stick_x, input_code::pad_right_stick_left, input_code::pad_right_stick_right);
        detail::derive_axis(dev, input_code::pad_right_stick_y, input_code::pad_right_stick_down, input_code::pad_right_stick_up);

        detail::derive_button_axis(dev, input_code::pad_dpad_left, input_code::pad_dpad_right, input_code::pad_dpad_x);
        detail::derive_button_axis(dev, input_code::pad_dpad_down, input_code::pad_dpad_up, input_code::pad_dpad_y);
    }
}
