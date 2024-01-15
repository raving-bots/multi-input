// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <vector>

#include "utils.hpp"

namespace multi_input {
    struct context;
    struct device;

    struct enumeration {
        explicit enumeration(context* ctx);
        enumeration(const enumeration&);
        enumeration(enumeration&&);
        enumeration& operator=(const enumeration&) = delete;
        enumeration& operator=(enumeration&&) = delete;

        device* next();
        void reset();
    private:
        context* m_ctx;
        std::vector<device*> m_devices;
        std::vector<device*>::iterator m_current;
        std::vector<device*>::iterator m_end;
    };
}
