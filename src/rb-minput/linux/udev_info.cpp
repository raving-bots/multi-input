// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <fstream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "linux/udev_info.hpp"
#include "linux/fs.hpp"

namespace multi_input {
    namespace lnx {
        udev_info::udev_info(std::string name) :
            m_db_path(fs::join("/run/udev/data", name)),
            m_properties()
        {
            std::ifstream fp{m_db_path};
            std::string line{};

            while (std::getline(fp, line)) {
                if (boost::starts_with(line, "E:")) {
                    line = line.substr(2);
                    std::vector<std::string> parts{};

                    boost::split(parts, line, [](char c) -> bool { return c == '='; });
                    if (parts.size() != 2) {
                        continue;
                    }

                    m_properties.emplace(parts[0], parts[1]);
                }
            }
        }
    }
}
