// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xlibint.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>
#include <sys/select.h>

#ifdef min
#   undef min
#endif

#ifdef max
#   undef max
#endif
