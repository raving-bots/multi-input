// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "utils.hpp"

namespace multi_input {
    struct context;

    struct source {
        RB_NON_MOVEABLE(source);

        virtual ~source() {
        }

        virtual void drain_events() = 0;
        virtual void enum_devices() = 0;
    protected:
        explicit source(context* ctx) : m_ctx(ctx) {}

        context *m_ctx;
    };
}
