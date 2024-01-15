// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <string>
#include <boost/format.hpp>

namespace multi_input {
    namespace detail {
        inline std::string format(boost::format& fmt) {
            return fmt.str();
        }

        template <typename T, typename... Args>
        std::string format(boost::format& fmt, T&& arg, Args&&... args) {
            return format(fmt % std::forward<T>(arg), std::forward<Args>(args)...);
        }
    }

    template <typename... Args>
    std::string format(std::string fmt_str, Args&&... args) {
        boost::format fmt{fmt_str};
        return detail::format(fmt, std::forward<Args>(args)...);
    }
}
