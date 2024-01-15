// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "linux/file_descriptor.hpp"
#include "linux/poller.hpp"
#include "source.hpp"
#include "utils.hpp"
#include "api_types.hpp"

namespace multi_input {
    struct context;

    namespace lnx {
        struct evdev_device;
        struct evdev_handle;

        struct evdev_device_map {
            RB_COPYABLE(evdev_device_map);

            evdev_device_map() : m_name_to_id(), m_id_to_name(), m_fd_to_id(), m_id_to_fd() {}

            void add(const std::string& name, int fd, device_id id) {
                m_name_to_id.emplace(name, id);
                m_id_to_name.emplace(id, name);
                m_fd_to_id.emplace(fd, id);
                m_id_to_fd.emplace(id, fd);
            }

            void remove(const std::string& name, int fd, device_id id) {
                m_name_to_id.erase(name);
                m_id_to_name.erase(id);
                m_fd_to_id.erase(fd);
                m_id_to_fd.erase(id);
            }

            void clear() {
                m_name_to_id.clear();
                m_id_to_name.clear();
                m_fd_to_id.clear();
                m_id_to_fd.clear();
            }

            device_id* name_to_id(const std::string& name) {
                auto it = m_name_to_id.find(name);
                return it == m_name_to_id.end() ? nullptr : &it->second;
            }

            std::string* id_to_name(device_id id) {
                auto it = m_id_to_name.find(id);
                return it == m_id_to_name.end() ? nullptr : &it->second;
            }

            device_id* fd_to_id(int fd) {
                auto it = m_fd_to_id.find(fd);
                return it == m_fd_to_id.end() ? nullptr : &it->second;
            }

            int* id_to_fd(device_id id) {
                auto it = m_id_to_fd.find(id);
                return it == m_id_to_fd.end() ? nullptr : &it->second;
            }

            auto begin() {
                return m_name_to_id.begin();
            }

            auto end() {
                return m_name_to_id.end();
            }
        private:
            std::unordered_map<std::string, device_id> m_name_to_id;
            std::unordered_map<device_id, std::string> m_id_to_name;
            std::unordered_map<int, device_id> m_fd_to_id;
            std::unordered_map<device_id, int> m_id_to_fd;
        };

        struct evdev_source : source {
            RB_NON_MOVEABLE(evdev_source);

            explicit evdev_source(context*);
            virtual ~evdev_source();
            virtual void drain_events() override;
            virtual void enum_devices() override;
        private:
            void add_device(const std::string&);
            void remove_device(const std::string&);
            evdev_device* get_device(int);
            void process_inotify();
            void process_inotify_udev();
            void process_device(evdev_device&);

            evdev_device_map m_device_map;
            file_descriptor m_inotify;
            file_descriptor m_inotify_udev;
            poller m_poller;
            std::string m_sysfs_base_path;
            std::vector<std::string> m_pending;
        };
    }
}
