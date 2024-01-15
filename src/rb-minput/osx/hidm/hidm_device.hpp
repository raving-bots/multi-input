// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "device.hpp"
#include "utils.hpp"

#include <IOKit/hid/IOHIDManager.h>
#include <ForceFeedback/ForceFeedback.h>

namespace multi_input {
    struct context;

    namespace osx {
        struct hidm_device : device {
            RB_NON_MOVEABLE(hidm_device);

            hidm_device(context*, device_id, std::string, IOHIDDeviceRef);
            virtual ~hidm_device();

            virtual void commit() override;
            virtual bool vibrate(int, float, float) override;

            virtual bool can_vibrate() const override {
                return m_can_vibrate;
            }
        private:
            void on_input(IOHIDValueRef);
            void add_element(IOHIDElementRef, uint32_t, uint32_t);
            void add_gamepad_element(IOHIDElementRef);
            void add_keyboard_element(IOHIDElementRef);
            void add_mouse_element(IOHIDElementRef);
            void add_axis(IOHIDElementRef, input_code);

            IOHIDDeviceRef m_handle;
            std::unordered_map<IOHIDElementCookie, input_code> m_axis_map;
            FFDeviceObjectReference m_feedback_handle;
            FFEffectObjectReference m_last_effect;
            FFCAPABILITIES m_feedback_caps;
            bool m_can_vibrate;
        };
    }
}
