// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <unordered_map>
#include <string>

#include "utils.hpp"

namespace multi_input {
    namespace lnx {
        struct udev_info {
            RB_COPYABLE(udev_info);

            explicit udev_info(std::string);

            auto find(const std::string& property) const {
                return m_properties.find(property);
            }

            auto begin() const {
                return m_properties.begin();
            }

            auto end() const {
                return m_properties.end();
            }

            auto get(const std::string& property) const {
                auto it = find(property);
                if (it == end()) {
                    return std::string{};
                }
                return it->second;
            }

            bool is_tagged(const std::string& property) const {
                auto it = find(property);
                return it != m_properties.end() && it->second == "1";
            }
        private:
            std::string m_db_path;
            std::unordered_map<std::string, std::string> m_properties;
        };
    }
}
