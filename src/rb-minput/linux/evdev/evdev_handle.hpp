// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "linux/file_descriptor.hpp"
#include "linux/udev_info.hpp"
#include "utils.hpp"

struct libevdev;

namespace multi_input {
    namespace lnx {
        struct evdev_handle {
            RB_NON_COPYABLE(evdev_handle);

            evdev_handle(std::string, std::string, udev_info);
            evdev_handle(evdev_handle&&);
            evdev_handle& operator=(evdev_handle&&) = delete;
            ~evdev_handle();

            libevdev* get() {
                return m_handle;
            }

            int get_fd() {
                return m_device.get();
            }

            const std::string& get_symbolic_name() const {
                return m_symbolic_name;
            }

            const std::string& get_path() const {
                return m_path;
            }

            const udev_info& get_info() const {
                return m_info;
            }
        private:
            file_descriptor m_device;
            libevdev *m_handle;
            std::string m_symbolic_name;
            udev_info m_info;
            std::string m_path;
        };
    }
}
