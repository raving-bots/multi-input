// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "device.hpp"
#include "utils.hpp"
#include "windows/winapi.hpp"

namespace multi_input {
    namespace windows {
        struct setup_device_info;

        struct raw_input_device : device {
            RB_NON_MOVEABLE(raw_input_device);

            raw_input_device(context*, device_id, void*, const RID_DEVICE_INFO&, const setup_device_info&);
            virtual ~raw_input_device();
            void update(const RAWINPUT&);
            virtual void commit() override;
        private:
            void update_mouse(const RAWMOUSE&);
            void update_mouse_button(input_code, unsigned short, unsigned short, unsigned short);
            void update_mouse_axis(input_code, float);
            void update_keyboard(const RAWKEYBOARD&);

            void *m_handle;
            bool m_has_last_input;
            float m_last_x;
            float m_last_y;
        };
    }
}
