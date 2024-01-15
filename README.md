# Multi-Input

Multi-Input is an input middleware that enables the developer to independently query input devices (mice, keyboards, and gamepads)
from multiple sources on Windows, Linux, and macOS. The library consists of a native platform-specific code, and a C# wrapper for Unity.

> [!WARNING]
> This library is no longer supported or maintained, and is not recommended for use in new projects. This open source release is for the benefit of projects already using it.

## Native requirements

* C++14 compiler
* CMake 3.22+
* Linux: evdev, X11 and XCB headers (Ubuntu: `libevdev-dev`, `libx11-dev`, `libx11-xcb-dev`, `libxcb1-dev`)

> [!WARNING]
> macOS platform support code may fail to work correctly in some cases due to underlying platform changes. The codebase also predates the ARM switch.

Run CMake as usual to build. `rb-minput` is the main library, `rb-minput-test` is a small utility that uses it to print out input events.

## Using the C# code

We recommend including the code directly in your project, or creating a new package to contain it. Remove any existing `RavingBots.MultiInput.*` assemblies.
The code has not been tested with Unity newer than 2019.3.0f3.

> [!WARNING]
> The C# code for Unity involves a lot of unnecessary allocations. We recommend replacing it entirely with custom wrappers over the native PInvoke calls.
