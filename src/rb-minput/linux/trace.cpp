// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <iostream>

#include "utils.hpp"

namespace multi_input {
    namespace debug {
        void tracer::output(const char* message) {
            std::cerr << "\x1B[1;30m(" << m_function << " @ " << m_file << ":" << m_line << ")\n\t" << message << "\x1B[0m\n";
        }

        void tracer::enter() {
        }

        void tracer::leave() {
        }
    }
}
