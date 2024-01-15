// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "linux/xi2/x11.hpp"
#include "utils.hpp"

namespace multi_input {
    namespace lnx {
        struct x11_device_query {
            RB_NON_COPYABLE(x11_device_query);
            x11_device_query& operator=(x11_device_query&&) = delete;

            x11_device_query(Display *display, int query_id) :
                m_size(), m_data(XIQueryDevice(display, query_id, &m_size))
            {
            }

            x11_device_query(x11_device_query&& other) : m_size(other.m_size), m_data(other.m_data) {
                other.m_data = nullptr;
            }

            ~x11_device_query() {
                if (m_data != nullptr) {
                    XIFreeDeviceInfo(m_data);
                }
            }

            XIDeviceInfo* begin() {
                return m_data;
            }

            XIDeviceInfo* end() {
                if (m_data == nullptr) {
                    return nullptr;
                }

                return m_data + m_size;
            }
        private:
            int m_size;
            XIDeviceInfo *m_data;
        };
    }
}
