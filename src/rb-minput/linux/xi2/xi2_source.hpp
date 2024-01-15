// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <unordered_map>

#include "source.hpp"
#include "utils.hpp"

#include "linux/xi2/x11.hpp"
#include "linux/xi2/x11_display.hpp"
#include "api_types.hpp"

namespace multi_input {
    struct context;

    namespace lnx {
        struct xi2_device;

        struct xi2_source : source {
            RB_NON_MOVEABLE(xi2_source);

            explicit xi2_source(context*);
            virtual ~xi2_source();
            virtual void drain_events() override;
            virtual void enum_devices() override;
        private:
            bool has_next_event();
            void add_device(XIDeviceInfo&);
            void remove_device(int);
            xi2_device* get_device(int);
            void on_device_event(XIRawEvent*);
            void on_hierarchy_event(XIHierarchyEvent*);

            int m_opcode;
            int m_first_event;
            int m_first_error;
            x11_display m_display;
            Window m_root_window;
            std::unordered_map<int, device_id> m_device_map;
        };
    }
}
