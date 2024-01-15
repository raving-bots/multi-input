// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <array>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "windows/winapi.hpp"
#include "windows/raw_input/raw_input_source.hpp"
#include "windows/raw_input/raw_input_device.hpp"
#include "windows/setupdi.hpp"
#include "context.hpp"

namespace multi_input {
    namespace windows {
        constexpr USHORT hid_generic_desktop = 0x01;
        constexpr USHORT hid_keyboard = 0x06;
        constexpr USHORT hid_mouse = 0x02;
        const wchar_t *wnd_class_name = L"RavingBots.RawInput";

        std::vector<std::wstring> split_raw_name(std::wstring raw_name) {
            raw_name = raw_name.substr(4);

            std::vector<std::wstring> result;
            result.reserve(4);

            boost::split(result, raw_name, [](wchar_t x) { return x == L'#'; });
            result.resize(result.size() - 1);

            return result;
        }

        std::wstring make_instance_id(const std::vector<std::wstring>& parts) {
            auto result = boost::join(parts, L"\\");
            boost::to_lower(result);
            return result;
        }

        std::wstring get_raw_name(HANDLE device) {
            UINT size = 0;

            if (GetRawInputDeviceInfo(device, RIDI_DEVICENAME, nullptr, &size) != 0) {
                throw_winapi_error("Failed to query buffer size for device name for device %p", reinterpret_cast<UINT_PTR>(device));
            }

            std::wstring raw_name;
            raw_name.resize(size);

            if (GetRawInputDeviceInfo(device, RIDI_DEVICENAME, &raw_name[0], &size) != size) {
                throw_winapi_error("Failed to get the device name for device %p", reinterpret_cast<UINT_PTR>(device));
            }

            raw_name.resize(raw_name.size() - 1);
            return raw_name;
        }

        LRESULT CALLBACK on_window_message(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
            RB_TRACE_ENTER();

            auto self = reinterpret_cast<raw_input_source*>(GetWindowLongPtr(window, GWLP_USERDATA));

            if (self != nullptr) {
                if (message == WM_INPUT) {
                    RB_TRACE("WM_INPUT");
                    self->dispatch(reinterpret_cast<HRAWINPUT>(lparam));
                } else if (message == WM_INPUT_DEVICE_CHANGE) {
                    RB_TRACE("WM_INPUT_DEVICE_CHANGE");
                    auto attached = wparam == GIDC_ARRIVAL;
                    self->dispatch_state(reinterpret_cast<void*>(lparam), attached);
                }
            } else {
                RB_TRACE("self == nullptr");
            }

            return DefWindowProc(window, message, wparam, lparam);
        }

        raw_input_source::raw_input_source(context* ctx) : source(ctx), m_window(), m_device_map() {
            RB_TRACE_ENTER();

            WNDCLASS wnd_class{};
            wnd_class.lpszClassName = wnd_class_name;
            wnd_class.lpfnWndProc = on_window_message;

            auto atom = RegisterClass(&wnd_class);

            if (atom == 0) {
                auto error = GetLastError();
                if (error != ERROR_CLASS_ALREADY_EXISTS) {
                    throw_winapi_error(error, "Failed to register raw input window class");
                }
            }

            m_window = CreateWindowEx(
                0, wnd_class_name, L"", 0, 0, 0, 0, 0,
                HWND_MESSAGE, nullptr, nullptr, nullptr);

            if (m_window == nullptr) {
                throw_winapi_error("Failed to create raw input message window");
            }

            SetWindowLongPtr(m_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

            USHORT flags = RIDEV_DEVNOTIFY | RIDEV_INPUTSINK;
            std::array<RAWINPUTDEVICE, 2> devices = {{
                {hid_generic_desktop, hid_mouse, flags, m_window},
                {hid_generic_desktop, hid_keyboard, flags, m_window}
            }};

            if (!RegisterRawInputDevices(devices.data(), devices.size(), sizeof(RAWINPUTDEVICE))) {
                throw_winapi_error("Failed to register for raw input");
            }
        }

        raw_input_source::~raw_input_source() {
            if (m_window) {
                DestroyWindow(m_window);
            }
        }

        void raw_input_source::drain_events() {
            // no-op
        }

        void raw_input_source::enum_devices() {
            RB_TRACE_ENTER();

            // while Windows doesn't seem to reuse the device handles, we want
            // consistent behaviour wrt Linux
            for (auto&& pair : m_device_map) {
                m_ctx->remove_device(pair.second);
            }

            m_device_map.clear();

            UINT size = 0;
            std::vector<RAWINPUTDEVICELIST> ri_devices;

            if (GetRawInputDeviceList(nullptr, &size, sizeof(RAWINPUTDEVICELIST)) != 0) {
                throw_winapi_error("Failed to query buffer size for RI device list");
            }

            ri_devices.resize(size);

            if (GetRawInputDeviceList(&ri_devices[0], &size, sizeof(RAWINPUTDEVICELIST)) != size) {
                throw_winapi_error("Failed to get the RI device list");
            }

            auto found_devices = enumerate_devices(m_ctx);

            for (auto&& device : ri_devices) {
                add_device(device.hDevice, found_devices);
            }
        }

        void raw_input_source::add_device(void* handle) {
            RB_TRACE_ENTER();

            if (m_device_map.find(handle) != m_device_map.end()) {
                RB_TRACE("skipping device: already exists");
                return;
            }

            auto found_devices = enumerate_devices(m_ctx);
            add_device(handle, found_devices);
        }

        void raw_input_source::add_device(void* handle, const std::unordered_map<std::wstring, setup_device_info>& found_devices) {
            RB_TRACE_ENTER();

            if (m_device_map.find(handle) != m_device_map.end()) {
                RB_TRACE("skipping device: already exists");
                return;
            }

            auto raw_name = get_raw_name(handle);
            auto raw_name_parts = split_raw_name(raw_name);
            auto instance_id = make_instance_id(raw_name_parts);
            auto it = found_devices.find(instance_id);

            if (it == found_devices.end()) {
                m_ctx->log_warning("ri: RI device %p (%s) doesn't have a found_devices entry", handle, to_utf8(instance_id));
                return;
            }

            RB_TRACE("grabbing info");
            auto device_info = it->second;

            RID_DEVICE_INFO info{};
            UINT size = sizeof(info);
            info.cbSize = size;

            if (GetRawInputDeviceInfo(handle, RIDI_DEVICEINFO, &info, &size) != size) {
                throw_winapi_error("Failed to get the device info for device %p", reinterpret_cast<UINT_PTR>(handle));
            }

            if (info.dwType != RIM_TYPEKEYBOARD && info.dwType != RIM_TYPEMOUSE) {
                RB_TRACE("skipping device: neither mouse not keyboard");
                return;
            }

            RB_TRACE("creating new device object");
            auto id = m_ctx->get_next_id();
            m_ctx->add_device(std::unique_ptr<device>{new raw_input_device(m_ctx, id, handle, info, device_info)});
            m_device_map.emplace(handle, id);
        }

        void raw_input_source::remove_device(void* handle) {
            RB_TRACE_ENTER();

            auto map_it = m_device_map.find(handle);
            if (map_it == m_device_map.end()) {
                RB_TRACE("map_it == end");
                return;
            }

            RB_TRACE("removing device object");
            m_ctx->remove_device(map_it->second);
            m_device_map.erase(map_it);
        }

        raw_input_device* raw_input_source::get_device(void* handle) {
            RB_TRACE_ENTER();

            auto it = m_device_map.find(handle);
            if (it == m_device_map.end()) {
                RB_TRACE("unknown device: not in m_device_map");
                return nullptr;
            }

            auto device_ptr = static_cast<raw_input_device*>(m_ctx->get_device(it->second));
            if (device_ptr == nullptr) {
                RB_TRACE("unknown device: not in context devices");
                return nullptr;
            }

            return device_ptr;
        }

        void raw_input_source::dispatch(HRAWINPUT input) {
            RB_TRACE_ENTER();

            if (input == nullptr) {
                RB_TRACE("input == nullptr");
                return;
            }

            m_ctx->log_verbose("ri: WM_INPUT: %p", input);

            UINT size = 0;

            if (GetRawInputData(input, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) != 0) {
                throw_winapi_error("Failed to query buffer size for incoming raw input data");
            }

            std::vector<BYTE> buffer;
            buffer.resize(size);

            if (GetRawInputData(input, RID_INPUT, &buffer[0], &size, sizeof(RAWINPUTHEADER)) != size) {
                throw_winapi_error("Failed to get incoming raw input");
            }

            auto packet = reinterpret_cast<RAWINPUT*>(&buffer[0]);
            auto device = get_device(packet->header.hDevice);

            if (device == nullptr) {
                RB_TRACE("unknown device");
                return;
            }

            device->update(*packet);
        }

        void raw_input_source::dispatch_state(void* handle, bool attached) {
            m_ctx->log_debug("raw_input: device %p new state: %s", handle, attached ? "attached" : "detached");

            if (!attached) {
                remove_device(handle);
            } else {
                add_device(handle);
            }
        }
    }
}
