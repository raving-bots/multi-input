// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "linux/posix.hpp"
#include "linux/poller.hpp"

namespace multi_input {
    namespace lnx {
        poller::poller() : m_fds() {}

        void poller::add(int fd) {
            m_fds.emplace_back(pollfd{ fd, POLLIN, 0 });
        }

        void poller::remove(int fd) {
            auto it = std::find_if(m_fds.begin(), m_fds.end(), [&](const pollfd& p) {
                return p.fd == fd;
            });

            if (it != m_fds.end()) {
                m_fds.erase(it);
            }
        }

        bool poller::poll() {
            auto rc = ::poll(m_fds.data(), m_fds.size(), 0);
            if (rc < 0) {
                throw_posix_error("Failed to poll devices");
            }
            return rc > 0;
        }

        std::vector<int> poller::get_ready() {
            std::vector<int> ready{};

            for (auto&& entry : m_fds) {
                if ((entry.revents & POLLIN) == 0) {
                    continue;
                }

                ready.push_back(entry.fd);
            }

            return ready;
        }
    }
}
