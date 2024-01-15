// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <iostream>

#include "utils.hpp"
#include "windows/winapi.hpp"

namespace multi_input {
    namespace debug {
        void tracer::output(const char* message) {
            OutputDebugStringA("[Multi-Input] (");
            OutputDebugStringA(m_function);
            OutputDebugStringA(" @ ");
            OutputDebugStringA(m_file);
            OutputDebugStringA(":");
            OutputDebugStringA(m_line);
            OutputDebugStringA("): ");
            OutputDebugStringA(message);
            OutputDebugStringA("\n");

            std::cerr << m_function << " @ " << m_file << ":" << m_line << ")\n\t" << message << "\n";
        }

        void tracer::enter() {
        }

        void tracer::leave() {
        }
    }
}
