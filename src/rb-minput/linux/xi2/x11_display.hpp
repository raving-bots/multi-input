// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "linux/xi2/x11.hpp"
#include "utils.hpp"

namespace multi_input {
    namespace lnx {
        struct x11_display {
            RB_NON_COPYABLE(x11_display);
            x11_display& operator=(x11_display&&) = delete;

            x11_display() : m_display(XOpenDisplay(nullptr)) {
                if (m_display == nullptr) {
                    throw std::runtime_error("Failed to connect to X11");
                }
            }

            x11_display(x11_display&& other) : m_display(other.m_display) {
                other.m_display = nullptr;
            }

            ~x11_display() {
                if (m_display != nullptr) {
                    XCloseDisplay(m_display);
                }
            }

            Display* get() {
                return m_display;
            }
        private:
            Display* m_display;
        };
    }
}
