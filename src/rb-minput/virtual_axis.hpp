// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

namespace multi_input {
    struct virtual_axis {
        RB_COPYABLE(virtual_axis);

        virtual_axis() : m_current(0), m_previous(0), m_next(0) {}

        void set(float value) {
            m_next = value;
        }

        void add(float value) {
            m_next += value;
        }

        void commit() {
            m_previous = m_current;
            m_current = m_next;
        }

        float get() const {
            return m_current;
        }

        float get_previous() const {
            return m_previous;
        }

        float get_next() const {
            return m_next;
        }
    private:
        float m_current;
        float m_previous;
        float m_next;
    };
}
