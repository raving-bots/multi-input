// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <algorithm>

#include "enumeration.hpp"
#include "device.hpp"
#include "context.hpp"

namespace multi_input {
    enumeration::enumeration(context* ctx)
        : m_ctx(ctx), m_devices(ctx->get_devices()), m_current(), m_end()
    {
        reset();
    }

    enumeration::enumeration(const enumeration& other)
        : m_ctx(other.m_ctx), m_devices(other.m_devices), m_current(), m_end()
    {
        reset();
    }

    enumeration::enumeration(enumeration&& other)
        : m_ctx(other.m_ctx), m_devices(other.m_devices), m_current(), m_end()
    {
        reset();
    }

    device* enumeration::next() {
        if (m_current == m_end) {
            return nullptr;
        }

        auto result = *m_current;
        ++m_current;
        return result;
    }

    void enumeration::reset() {
        m_current = std::begin(m_devices);
        m_end     = std::end(m_devices);
    }
}
