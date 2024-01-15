// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <vector>
#include <algorithm>

#include "utils.hpp"

#include <poll.h>

namespace multi_input {
    namespace lnx {
        struct poller {
            RB_MOVEABLE(poller);

            poller();

            void add(int fd);
            void remove(int fd);
            bool poll();
            std::vector<int> get_ready();
        private:
            std::vector<pollfd> m_fds;
        };
    }
}
