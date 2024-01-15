// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <memory>

#include "source.hpp"
#include "utils.hpp"
#include "api_types.hpp"

#include <IOKit/hid/IOHIDManager.h>

namespace multi_input {
    struct context;

    namespace osx {
        struct hidm_device;

        struct hid_manager {
            RB_NON_MOVEABLE(hid_manager);

            explicit hid_manager(context*);
            ~hid_manager();

            void open();
            void close();
            void reopen();

            void schedule();
            void unschedule();

            IOHIDManagerRef get() {
                return m_ref;
            }
        private:
            context* m_ctx;
            IOHIDManagerRef m_ref;
            bool m_open;
            bool m_scheduled;
        };

        struct hidm_source : source {
            RB_NON_MOVEABLE(hidm_source);

            explicit hidm_source(context*);
            virtual ~hidm_source();

            virtual void drain_events() override;
            virtual void enum_devices() override;
        private:
            void add_device(IOHIDDeviceRef);
            void remove_device(IOHIDDeviceRef);
            hidm_device* get_device(IOHIDDeviceRef);

            hid_manager m_hid_manager;
            std::unordered_map<IOHIDDeviceRef, device_id> m_device_map;
        };
    }
}
