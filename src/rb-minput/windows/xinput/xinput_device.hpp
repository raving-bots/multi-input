// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <chrono>

#include "device.hpp"
#include "utils.hpp"
#include "api_types.hpp"
#include "windows/winapi.hpp"

namespace multi_input {
    namespace windows {
        struct xinput_device : device {
            RB_NON_MOVEABLE(xinput_device);

            xinput_device(context*, device_id, int);
            virtual ~xinput_device();
            void update();
            virtual void commit() override;
            virtual bool vibrate(int, float, float) override;

            virtual bool can_vibrate() const override {
                return is_usable();
            }
        private:
            void update_axis(input_code, float);
            void update_button(input_code, unsigned short);
            bool do_vibrate(float, float);

            int m_index;
            XINPUT_STATE m_state;
            std::chrono::milliseconds m_last_effect_duration;
            std::chrono::steady_clock::time_point m_last_effect_start;
            bool m_last_effect_playing;
        };
    }
}
