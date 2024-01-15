// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "device.hpp"
#include "utils.hpp"
#include "linux/xi2/x11.hpp"

namespace multi_input {
    namespace lnx {
        namespace detail {
            struct atoms;
        }

        struct xi2_device : device {
            RB_NON_MOVEABLE(xi2_device);

            xi2_device(context*, device_id, Display*, XIDeviceInfo&);
            virtual ~xi2_device();
            void update(XIRawEvent&);
            virtual void commit() override;
        private:
            void update_key(XIRawEvent&);
            void update_motion(XIRawEvent&);
            void update_button(XIRawEvent&);
            void add_valuator_class(XIValuatorClassInfo&, const detail::atoms&);
            void add_button_class(XIButtonClassInfo&);
            void add_key_class(XIKeyClassInfo&);
            input_code map_key_code(int);
            input_code map_button_code(int);

            int m_x11_id;
            Display *m_display;
            int m_axis_rel_x;
            int m_axis_rel_y;
            int m_axis_rel_horiz_wheel;
            int m_axis_rel_vert_wheel;
        };
    }
}
