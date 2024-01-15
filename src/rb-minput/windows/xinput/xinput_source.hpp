// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <array>

#include "source.hpp"
#include "utils.hpp"
#include "api_types.hpp"

namespace multi_input {
    namespace windows {
        struct xinput_device;

        struct xinput_source : source {
            RB_NON_MOVEABLE(xinput_source);

            explicit xinput_source(context*);
            virtual ~xinput_source();

            virtual void drain_events() override;
            virtual void enum_devices() override;
        private:
            bool m_created_devices;
            std::array<xinput_device*, 4> m_devices;
        };
    }
}
