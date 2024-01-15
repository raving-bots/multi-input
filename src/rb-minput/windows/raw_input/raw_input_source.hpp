// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <unordered_map>

#include "source.hpp"
#include "utils.hpp"
#include "api_types.hpp"

struct HWND__;
struct HRAWINPUT__;

namespace multi_input {
    struct context;

    namespace windows {
        struct raw_input_device;
        struct setup_device_info;

        struct raw_input_source : source {
            RB_NON_MOVEABLE(raw_input_source);

            explicit raw_input_source(context*);
            virtual ~raw_input_source();

            void dispatch(HRAWINPUT__*);
            void dispatch_state(void*, bool);

            virtual void drain_events() override;
            virtual void enum_devices() override;
        private:
            void add_device(void*);
            void add_device(void*, const std::unordered_map<std::wstring, setup_device_info>&);
            void remove_device(void*);
            raw_input_device* get_device(void*);

            HWND__ *m_window;
            std::unordered_map<void*, device_id> m_device_map;
        };
    }
}
