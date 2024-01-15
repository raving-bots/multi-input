// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <string>

#include <IOKit/hid/IOHIDManager.h>

namespace multi_input {
    namespace osx {
        std::string get_collection_type(IOHIDElementRef);
        std::string get_element_type(IOHIDElementRef);
        std::string get_usage(IOHIDElementRef);
        std::string get_usage_page(IOHIDElementRef);
        std::string describe_element(IOHIDElementRef, int, bool);
    }
}
