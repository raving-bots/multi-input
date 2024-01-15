// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "windows/winapi.hpp"
#include "windows/xinput/xinput_source.hpp"
#include "windows/xinput/xinput_device.hpp"
#include "context.hpp"

namespace multi_input {
    namespace windows {
        xinput_source::xinput_source(context* ctx) : source(ctx), m_created_devices(), m_devices() {
        }

        xinput_source::~xinput_source() {
        }

        void xinput_source::enum_devices() {
            RB_TRACE_ENTER();

            // XInput has static number of devices, so there's no removing or re-enumerating
            if (m_created_devices) {
                RB_TRACE("already set up");
                return;
            }

            for (int index = 0; index < 4; index++) {
                RB_TRACE("creating new device object");
                auto id = m_ctx->get_next_id();
                m_ctx->add_device(std::unique_ptr<device>{new xinput_device(m_ctx, id, index)});
                m_devices[index] = static_cast<xinput_device*>(m_ctx->get_device(id));
            }

            m_created_devices = true;
        }

        void xinput_source::drain_events() {
            for (auto&& device : m_devices) {
                device->update();
            }
        }
    }
}
