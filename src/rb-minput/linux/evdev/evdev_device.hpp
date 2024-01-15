// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "device.hpp"
#include "utils.hpp"
#include "linux/evdev/evdev_handle.hpp"

struct input_event;

namespace multi_input {
    namespace lnx {
        struct evdev_device : device {
            RB_NON_MOVEABLE(evdev_device);

            evdev_device(context*, device_id, evdev_handle&&);
            virtual ~evdev_device();

            void update(const input_event&);
            void post_update();
            virtual bool vibrate(int, float, float) override;
            virtual void commit() override;

            evdev_handle& get_handle() {
                return m_handle;
            }

            virtual bool can_vibrate() const override {
                return m_can_vibrate;
            }
        private:
            bool try_add_axis(unsigned, unsigned);
            void update_button(unsigned, int);
            void update_axis(unsigned, int);
            input_code map_button_code(unsigned);
            input_code map_axis_code(unsigned);
            float map_value(unsigned, float);
            float get_deadzone_for(unsigned);

            evdev_handle m_handle;
            bool m_can_vibrate;
            short m_last_effect;
        };
    }
}
