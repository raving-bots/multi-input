// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "windows/winapi.hpp"
#include "windows/xinput/xinput_device.hpp"
#include "context.hpp"
#include "axis_utils.hpp"
#include "format.hpp"

namespace multi_input {
    namespace windows {
        xinput_device::xinput_device(context* ctx, device_id id, int index) :
            device(ctx, id),
            m_index(index), m_state(), m_last_effect_duration(), m_last_effect_start(),
            m_last_effect_playing()
        {
            auto& meta = get_meta();
            auto internal_id = format("xinput:%1%", index);
            meta.set_name(format("XInput Gamepad %1%", index + 1));
            meta.set_internal_id(internal_id);
            meta.set_location(internal_id);

            add_axis(input_code::pad_left_stick_up);
            add_axis(input_code::pad_left_stick_down);
            add_axis(input_code::pad_left_stick_left);
            add_axis(input_code::pad_left_stick_right);
            add_axis(input_code::pad_left_stick);
            add_axis(input_code::pad_left_stick_x);
            add_axis(input_code::pad_left_stick_y);
            add_axis(input_code::pad_right_stick_up);
            add_axis(input_code::pad_right_stick_down);
            add_axis(input_code::pad_right_stick_left);
            add_axis(input_code::pad_right_stick_right);
            add_axis(input_code::pad_right_stick);
            add_axis(input_code::pad_right_stick_x);
            add_axis(input_code::pad_right_stick_y);
            add_axis(input_code::pad_dpad_up);
            add_axis(input_code::pad_dpad_down);
            add_axis(input_code::pad_dpad_left);
            add_axis(input_code::pad_dpad_right);
            add_axis(input_code::pad_dpad_x);
            add_axis(input_code::pad_dpad_y);
            add_axis(input_code::pad_a);
            add_axis(input_code::pad_b);
            add_axis(input_code::pad_x);
            add_axis(input_code::pad_y);
            add_axis(input_code::pad_left_trigger);
            add_axis(input_code::pad_right_trigger);
            add_axis(input_code::pad_left_bumper);
            add_axis(input_code::pad_right_bumper);
            add_axis(input_code::pad_back);
            add_axis(input_code::pad_start);

            update();
        }

        xinput_device::~xinput_device() {
        }

        float get_deadzone_for(input_code code) {
            switch (code) {
                case input_code::pad_left_stick_x:
                case input_code::pad_left_stick_y:
                    return deadzone_left_stick;
                case input_code::pad_right_stick_x:
                case input_code::pad_right_stick_y:
                    return deadzone_right_stick;
                case input_code::pad_left_trigger:
                case input_code::pad_right_trigger:
                    return deadzone_trigger;
                default:
                    return 0;
            }
        }

        float get_max_for(input_code code) {
            switch (code) {
                case input_code::pad_left_stick_x:
                case input_code::pad_left_stick_y:
                case input_code::pad_right_stick_x:
                case input_code::pad_right_stick_y:
                    return 32767.0f;
                case input_code::pad_left_trigger:
                case input_code::pad_right_trigger:
                    return 255.0f;
                default:
                    return 0;
            }
        }

        float map_value(input_code code, float raw_value) {
            auto deadzone = get_deadzone_for(code);
            auto max = get_max_for(code);

            if (raw_value < 0) {
                return -apply_deadzone(-raw_value, max, deadzone);
            } else {
                return apply_deadzone(raw_value, max, deadzone);
            }
        }

        void xinput_device::update_axis(input_code code, float raw_value) {
            auto axis = get_axis(code);
            auto value = map_value(code, raw_value);
            axis->set(value);
        }

        void xinput_device::update_button(input_code code, unsigned short flag) {
            auto axis = get_axis(code);
            auto state = (m_state.Gamepad.wButtons & flag) != 0;
            axis->set(state ? 1.0f : 0.0f);
        }

        void xinput_device::update() {
            auto prev_packet = m_state.dwPacketNumber;
            set_usable(XInputGetState(m_index, &m_state) == ERROR_SUCCESS);

            if (m_last_effect_playing) {
                auto now = std::chrono::steady_clock::now();
                auto delta = now - m_last_effect_start;

                if (delta >= m_last_effect_duration) {
                    m_ctx->log_debug("xinput: last rumble expired, disabling motors");
                    do_vibrate(0, 0);
                    m_last_effect_playing = false;
                }
            }

            if (m_state.dwPacketNumber == prev_packet) {
                return;
            }

            m_ctx->log_verbose("xinput: controller %1% packet %2%", m_index, m_state.dwPacketNumber);

            update_axis(input_code::pad_left_stick_x,  m_state.Gamepad.sThumbLX);
            update_axis(input_code::pad_left_stick_y,  m_state.Gamepad.sThumbLY);
            update_axis(input_code::pad_right_stick_x, m_state.Gamepad.sThumbRX);
            update_axis(input_code::pad_right_stick_y, m_state.Gamepad.sThumbRY);
            update_axis(input_code::pad_left_trigger,  m_state.Gamepad.bLeftTrigger);
            update_axis(input_code::pad_right_trigger, m_state.Gamepad.bRightTrigger);

            update_button(input_code::pad_dpad_up,      XINPUT_GAMEPAD_DPAD_UP);
            update_button(input_code::pad_dpad_down,    XINPUT_GAMEPAD_DPAD_DOWN);
            update_button(input_code::pad_dpad_left,    XINPUT_GAMEPAD_DPAD_LEFT);
            update_button(input_code::pad_dpad_right,   XINPUT_GAMEPAD_DPAD_RIGHT);
            update_button(input_code::pad_start,        XINPUT_GAMEPAD_START);
            update_button(input_code::pad_back,         XINPUT_GAMEPAD_BACK);
            update_button(input_code::pad_left_stick,   XINPUT_GAMEPAD_LEFT_THUMB);
            update_button(input_code::pad_right_stick,  XINPUT_GAMEPAD_RIGHT_THUMB);
            update_button(input_code::pad_left_bumper,  XINPUT_GAMEPAD_LEFT_SHOULDER);
            update_button(input_code::pad_right_bumper, XINPUT_GAMEPAD_RIGHT_SHOULDER);
            update_button(input_code::pad_a,            XINPUT_GAMEPAD_A);
            update_button(input_code::pad_b,            XINPUT_GAMEPAD_B);
            update_button(input_code::pad_x,            XINPUT_GAMEPAD_X);
            update_button(input_code::pad_y,            XINPUT_GAMEPAD_Y);
        }

        void xinput_device::commit() {
            derive_stick_pre_commit(*this);
            device::commit();
        }

        bool xinput_device::do_vibrate(float left, float right) {
            XINPUT_VIBRATION effect{};
            effect.wLeftMotorSpeed = static_cast<WORD>(left * 65535);
            effect.wRightMotorSpeed = static_cast<WORD>(right * 65535);

            auto result = XInputSetState(m_index, &effect);
            if (result == ERROR_DEVICE_NOT_CONNECTED) {
                set_usable(false);
                return false;
            } else if (result != ERROR_SUCCESS) {
                throw_winapi_error(result, "Failed to send vibration command to gamepad %1%", m_index);
            }

            return true;
        }

        bool xinput_device::vibrate(int duration, float left, float right) {
            RB_TRACE_ENTER();

            if (!is_usable()) {
                RB_TRACE("cannot vibrate now");
                return false;
            }

            m_ctx->log_debug("xinput: vibrating gamepad %1% with force %2%/%3% for %4%ms", m_index, left, right, duration);

            m_last_effect_duration = std::chrono::milliseconds(duration);
            m_last_effect_start = std::chrono::steady_clock::now();
            m_last_effect_playing = do_vibrate(left, right);

            return m_last_effect_playing;
        }
    }
}
