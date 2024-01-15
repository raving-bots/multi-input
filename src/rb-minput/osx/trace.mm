// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "utils.hpp"

#import <Foundation/Foundation.h>

namespace multi_input {
    namespace debug {
        void tracer::output(const char* message) {
            NSLog(@"[Multi-Input] (%s @ %s:%s): %@", m_function, m_file, m_line, [NSString stringWithUTF8String:message]);
        }

        void tracer::enter() {
        }

        void tracer::leave() {
        }
    }
}
