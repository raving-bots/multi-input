// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <string>
#include <unordered_map>

#include "api_types.hpp"
#include "utils.hpp"

namespace multi_input {
    struct context;

    namespace windows {
        struct setup_device_info {
            RB_COPYABLE(setup_device_info);

            std::string m_friendly_name;
            std::string m_instance_id;

            int m_vendor_id{};
            int m_product_id{};
            int m_revision{};
            std::string m_serial;

            std::string m_location_id;

            setup_device_info() = default;
        };

        std::unordered_map<std::wstring, setup_device_info> enumerate_devices(context*);
    }
}
