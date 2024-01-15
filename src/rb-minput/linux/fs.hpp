// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <string>
#include <vector>

namespace multi_input {
    namespace lnx {
        namespace fs {
            bool exists(const std::string&);
            std::vector<std::string> list(const std::string&);
            std::string canonical(const std::string&);
            std::string parent(std::string);
            std::string filename(std::string);
            std::string sysfs_path();
            std::string read_all(std::string);

            inline std::string join(const std::string& a, const std::string& b) {
                return a + "/" + b;
            }
        }
    }
}
