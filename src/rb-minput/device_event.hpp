// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

namespace multi_input {
    enum class device_event : int {
        created = 1,
        removed = 2,
        unusable = 3,
        usable = 4,
    };
}
