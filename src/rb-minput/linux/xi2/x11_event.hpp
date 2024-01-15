// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "linux/xi2/x11.hpp"

namespace multi_input {
    namespace lnx {
        struct x11_event {
            RB_NON_COPYABLE(x11_event);
            x11_event& operator=(x11_event&&) = delete;

            x11_event(Display *display, int xi2_opcode) : m_display(display), m_event(), m_valid() {
                XNextEvent(m_display, &m_event);

                m_valid = (
                    m_event.xcookie.type == GenericEvent &&
                    m_event.xcookie.extension == xi2_opcode &&
                    XGetEventData(m_display, &m_event.xcookie)
                );
            }

            x11_event(x11_event&& other) :
                m_display(other.m_display), m_event(other.m_event), m_valid(other.m_valid)
            {
                other.m_valid = false;
            }

            ~x11_event() {
                if (m_valid) {
                    XFreeEventData(m_display, &m_event.xcookie);
                }
            }

            bool is_valid() {
                return m_valid;
            }

            int get_type() {
                return m_event.xcookie.evtype;
            }

            template <typename T>
            T* get_data() {
                return static_cast<T*>(m_event.xcookie.data);
            }
        private:
            Display *m_display;
            XEvent m_event;
            bool m_valid;
        };
    }
}
