// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "linux/file_descriptor.hpp"
#include "linux/posix.hpp"

namespace multi_input {
    namespace lnx {
        file_descriptor::file_descriptor(int fd) : m_fd(fd) {
        }

        file_descriptor::file_descriptor(file_descriptor&& other) : m_fd(other.m_fd) {
            other.m_fd = 0;
        }

        file_descriptor::~file_descriptor() {
            if (m_fd > 0) {
                close(m_fd);
            }
        }

        file_descriptor open_file_flags(const std::string& path, int flags) {
            auto fd = open(path.c_str(), flags);
            if (fd < 0) {
                throw_posix_error("Failed to open file %1% with flags %2%", path, flags);
            }
            return file_descriptor{fd};
        }

        file_descriptor open_file(const std::string& path) {
            return open_file_flags(path, O_RDONLY | O_CLOEXEC | O_NONBLOCK);
        }

        file_descriptor open_file_rw(const std::string& path) {
            return open_file_flags(path, O_RDWR | O_CLOEXEC | O_NONBLOCK);
        }

        file_descriptor open_inotify() {
            auto fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
            if (fd < 0) {
                throw_posix_error("Failed to create inotify instance");
            }
            return file_descriptor{fd};
        }

        file_descriptor open_null() {
            return open_file_flags("/dev/null", O_WRONLY | O_CLOEXEC | O_NONBLOCK);
        }
    }
}
