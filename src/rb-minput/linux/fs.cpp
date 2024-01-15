// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <iostream>
#include <sstream>
#include <fstream>

#include "linux/posix.hpp"
#include "linux/fs.hpp"
#include "utils.hpp"

namespace multi_input {
    namespace lnx {
        namespace fs {
            struct directory {
                RB_NON_MOVEABLE(directory);

                explicit directory(const std::string& path) : m_handle(opendir(path.c_str())) {
                    if (m_handle == nullptr) {
                        throw_posix_error("Failed to open directory: %1%", path);
                    }
                }

                ~directory() {
                    closedir(m_handle);
                }

                dirent* read() {
                    return readdir(m_handle);
                }
            private:
                DIR *m_handle;
            };

            struct realpath_buffer {
                RB_NON_MOVEABLE(realpath_buffer);

                explicit realpath_buffer(const std::string& path) : m_buf(realpath(path.c_str(), nullptr)) {
                    if (m_buf == nullptr) {
                        throw_posix_error("Failed to resolve path: %1%", path);
                    }
                }

                ~realpath_buffer() {
                    free(m_buf);
                }

                operator std::string() const {
                    return std::string{m_buf};
                }
            private:
                char *m_buf;
            };

            bool exists(const std::string& path) {
                struct stat s{};
                auto rc = ::stat(path.c_str(), &s);
                return rc == 0;
            }

            std::vector<std::string> list(const std::string& path) {
                std::vector<std::string> result{};

                directory dir{path};
                dirent *entry{};
                do {
                    if ((entry = dir.read()) != nullptr) {
                        std::string name = entry->d_name;
                        if (name == "." || name == "..") {
                            continue;
                        }

                        result.emplace_back(name);
                    }
                } while (entry != nullptr);

                return result;
            }

            std::string canonical(const std::string& path) {
                realpath_buffer buffer{path};
                return buffer;
            }

            std::string parent(std::string path) {
                return std::string{::dirname(&path[0])};
            }

            std::string filename(std::string path) {
                return std::string{::basename(&path[0])};
            }

            std::string sysfs_path() {
                if (fs::exists("/sys/subsystem")) {
                    return "/sys/subsystem/input/devices";
                } else {
                    return "/sys/class/input";
                }
            }

            // TODO actually I/O not fs but whatever
            std::string read_all(std::string path) {
                std::ifstream fp{path};
                std::stringstream ss;
                ss << fp.rdbuf();
                return ss.str();
            }
        }
    }
}
