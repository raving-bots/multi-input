// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <limits.h>
#include <dirent.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/inotify.h>

#ifdef min
#   undef min
#endif

#ifdef max
#   undef max
#endif

#include <stdexcept>
#include <type_traits>
#include <array>

#include "format.hpp"

namespace multi_input {
    namespace lnx {
        template <typename... Args>
        void throw_posix_error(int last_error, const std::string& fmt, Args&&... args) {
            std::array<char, 2048> error_buffer{};

            if (last_error < 0) {
                last_error = -last_error;
            }

            auto error_message = strerror_r(last_error, error_buffer.data(), error_buffer.size() - 1);
            auto message = format(fmt, std::forward<Args>(args)...);
            auto full_message = format("%1%: %2%", message, error_message);
            throw std::runtime_error(full_message);
        }

        template <typename... Args>
        void throw_posix_error(const std::string& fmt, Args&&... args) {
            throw_posix_error(errno, fmt, std::forward<Args>(args)...);
        }
    }
}
