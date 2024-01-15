cmake_minimum_required(VERSION 3.22)

project(multi_input)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig)
    find_package(X11 REQUIRED COMPONENTS xcb Xi)
    pkg_check_modules(EVDEV REQUIRED IMPORTED_TARGET libevdev)
endif()

add_library(rb-minput SHARED
    src/rb-minput/api.cpp
    src/rb-minput/api.hpp
    src/rb-minput/api_types.hpp
    src/rb-minput/axis_utils.cpp
    src/rb-minput/axis_utils.hpp
    src/rb-minput/context.cpp
    src/rb-minput/context.hpp
    src/rb-minput/device.cpp
    src/rb-minput/device.hpp
    src/rb-minput/device_event.hpp
    src/rb-minput/enumeration.cpp
    src/rb-minput/enumeration.hpp
    src/rb-minput/format.hpp
    src/rb-minput/input_code.hpp
    src/rb-minput/log_level.hpp
    src/rb-minput/source.hpp
    src/rb-minput/utils.hpp
    src/rb-minput/virtual_axis.hpp

    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/evdev/evdev.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/evdev/evdev_device.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/evdev/evdev_device.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/evdev/evdev_handle.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/evdev/evdev_handle.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/evdev/evdev_source.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/evdev/evdev_source.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/x11.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/x11_device_query.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/x11_display.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/x11_event.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/xi2_device.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/xi2_device.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/xi2_source.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/xi2/xi2_source.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/file_descriptor.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/file_descriptor.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/fs.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/fs.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/poller.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/poller.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/posix.hpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/trace.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/udev_info.cpp>
    $<$<PLATFORM_ID:Linux>:src/rb-minput/linux/udev_info.hpp>

    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/hidm/hidm_device.cpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/hidm/hidm_device.hpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/hidm/hidm_source.cpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/hidm/hidm_source.hpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/hid_debug.cpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/hid_debug.hpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/osx.cpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/osx.hpp>
    $<$<PLATFORM_ID:Darwin>:src/rb-minput/osx/trace.mm>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Array.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Array-Iterator.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-AutoPointer.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Boolean.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Data.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Data-Iterator.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Date.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Dictionary.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Dictionary-Iterator.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Error.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Number.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Pair.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-ReadStream.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-ReadStream-Iterator.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-String.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-String-Iterator.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-Type.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-URL.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-UUID.cpp>
    $<$<PLATFORM_ID:Darwin>:vendor/cfpp/CFPP-WriteStream.cpp>

    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/raw_input/raw_input_device.cpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/raw_input/raw_input_device.hpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/raw_input/raw_input_source.cpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/raw_input/raw_input_source.hpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/xinput/xinput_device.cpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/xinput/xinput_device.hpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/xinput/xinput_source.cpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/xinput/xinput_source.hpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/setupdi.cpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/setupdi.hpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/trace.cpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/winapi.cpp>
    $<$<PLATFORM_ID:Windows>:src/rb-minput/windows/winapi.hpp>
)

target_compile_features(rb-minput PUBLIC cxx_std_14)

target_include_directories(rb-minput PUBLIC
    src/rb-minput
    vendor
    vendor/cfpp
)

target_link_libraries(rb-minput PUBLIC
    $<$<PLATFORM_ID:Windows>:xinput9_1_0>
    $<$<PLATFORM_ID:Windows>:setupapi>
    $<$<PLATFORM_ID:Windows>:hid>
    $<$<PLATFORM_ID:Windows>:ole32>

    $<$<PLATFORM_ID:Linux>:X11::X11>
    $<$<PLATFORM_ID:Linux>:X11::Xi>
    $<$<PLATFORM_ID:Linux>:PkgConfig::EVDEV>
)

target_compile_definitions(rb-minput PUBLIC
    $<$<CONFIG:Debug>:RB_DEBUG>
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<NOT:$<CONFIG:Debug>>:RB_RELEASE>
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>

    $<$<PLATFORM_ID:Windows>:RB_PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Windows>:UNICODE>
    $<$<PLATFORM_ID:Windows>:_UNICODE>
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
    $<$<PLATFORM_ID:Windows>:_WINDOWS>
    $<$<PLATFORM_ID:Windows>:STRICT>

    $<$<PLATFORM_ID:Linux>:RB_PLATFORM_LINUX>
    $<$<PLATFORM_ID:Linux>:_GNU_SOURCE>

    $<$<PLATFORM_ID:Darwin>:RB_PLATFORM_OSX>
)

install(TARGETS rb-minput)

add_executable(rb-minput-test
    src/test/enums.hpp
    src/test/main.cpp
)

target_link_libraries(rb-minput-test PUBLIC rb-minput)

install(TARGETS rb-minput-test)

if(APPLE)
    target_compile_options(rb-minput PRIVATE -w)
    target_include_directories(rb-minput PUBLIC vendor/cfpp)

    target_link_libraries(rb-minput PUBLIC
        "-framework Foundation"
        "-framework IOKit"
        "-framework ForceFeedback"
        "-framework CoreFoundation"
    )

    add_executable(rb-minput-hidm-report src/osx-hid-report.cpp)
    target_link_libraries(rb-minput-hidm-report PUBLIC rb-minput)

    install(TARGETS rb-minput-hidm-report)
endif()
