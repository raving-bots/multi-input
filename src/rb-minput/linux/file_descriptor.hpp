// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "utils.hpp"

namespace multi_input {
    namespace lnx {
        struct file_descriptor {
            RB_NON_COPYABLE(file_descriptor);

            explicit file_descriptor(int);
            file_descriptor(file_descriptor&&);
            file_descriptor& operator=(file_descriptor&&) = delete;
            ~file_descriptor();

            int get() {
                return m_fd;
            }
        private:
            int m_fd;
        };

        file_descriptor open_file(const std::string&);
        file_descriptor open_file_rw(const std::string&);
        file_descriptor open_inotify();
        file_descriptor open_null();
    }
}
