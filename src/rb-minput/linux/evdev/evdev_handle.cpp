// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "linux/evdev/evdev_handle.hpp"
#include "linux/evdev/evdev.hpp"

namespace multi_input {
    namespace lnx {
        evdev_handle::evdev_handle(std::string path, std::string symbolic_name, udev_info info) :
            m_device(open_file_rw(path)), m_handle(), m_symbolic_name(symbolic_name), m_info(info), m_path(path)
        {
            if (libevdev_new_from_fd(m_device.get(), &m_handle) != 0) {
                m_handle = nullptr;
            }
        }

        evdev_handle::evdev_handle(evdev_handle&& other) :
            m_device(std::move(other.m_device)), m_handle(std::move(other.m_handle)),
            m_symbolic_name(std::move(other.m_symbolic_name)), m_info(std::move(other.m_info)),
            m_path(std::move(other.m_path))
        {
            other.m_handle = nullptr;
        }

        evdev_handle::~evdev_handle() {
            if (m_handle != nullptr) {
                libevdev_free(m_handle);
            }
        }
    }
}
