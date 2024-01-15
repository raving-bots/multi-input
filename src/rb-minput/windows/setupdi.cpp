// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <stdexcept>
#include <memory>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include "windows/winapi.hpp"
#include "windows/setupdi.hpp"
#include "context.hpp"

extern "C" {
#include <hidsdi.h>
}

#include <Objbase.h>

#ifdef interface
#   undef interface
#endif

namespace multi_input {
    namespace windows {
        struct property_not_found : virtual std::exception {
        };

        std::wstring get_device_property(HDEVINFO info_set, SP_DEVINFO_DATA& device, DWORD property, const std::wstring& device_id, bool can_fail) {
            DWORD size = 0;

            SetupDiGetDeviceRegistryProperty(info_set, &device, property, nullptr, nullptr, size, &size);
            if (size == 0) {
                auto error = GetLastError();
                if (can_fail && error == ERROR_INVALID_DATA) {
                    throw property_not_found{};
                }
                throw_winapi_error(error, "Failed to query buffer size for friendly name for %1%", to_utf8(device_id));
            }

            std::vector<char> buffer;
            buffer.resize(size);

            if (!SetupDiGetDeviceRegistryProperty(info_set, &device, property, nullptr, reinterpret_cast<PBYTE>(&buffer[0]), size, nullptr)) {
                throw_winapi_error("Failed to query friendly name for %1%", to_utf8(device_id));
            }

            return std::wstring(reinterpret_cast<wchar_t*>(&buffer[0]));
        }

        std::wstring get_device_friendly_name(HDEVINFO info_set, SP_DEVINFO_DATA& device, const std::wstring& device_id) {
            try {
                return get_device_property(info_set, device, SPDRP_FRIENDLYNAME, device_id, true);
            } catch (const property_not_found&) {
                try {
                    return get_device_property(info_set, device, SPDRP_DEVICEDESC, device_id, true);
                } catch (const property_not_found&) {
                    return L"";
                }
            }
        }

        std::wstring get_device_location(HDEVINFO info_set, SP_DEVINFO_DATA& device, const std::wstring& device_id) {
            try {
                return get_device_property(info_set, device, SPDRP_LOCATION_INFORMATION, device_id, true);
            } catch (const property_not_found&) {
                try {
                    return get_device_property(info_set, device, SPDRP_ADDRESS, device_id, true);
                } catch (const property_not_found&) {
                    return L"";
                }
            }
        }

        std::wstring get_device_instance_id(HDEVINFO info_set, SP_DEVINFO_DATA& device) {
            DWORD size = 0;

            SetupDiGetDeviceInstanceId(info_set, &device, nullptr, size, &size);
            if (size == 0) {
                throw_winapi_error("Failed to query buffer size for device instance ID");
            }

            std::vector<wchar_t> buffer;
            buffer.resize(size);

            if (!SetupDiGetDeviceInstanceId(info_set, &device, &buffer[0], size, nullptr)) {
                throw_winapi_error("Failed to query device instance ID");
            }

            auto result = std::wstring(buffer.begin(), buffer.end() - 1);
            boost::to_lower(result);
            return result;
        }

        std::string to_string(const GUID& guid) {
            std::wstring buffer{};
            buffer.resize(40);
            StringFromGUID2(guid, &buffer[0], 40);
            return to_utf8(buffer);
        }

        bool is_blacklisted(const std::wstring& instance_id) {
            // RDP devices are not usable and don't need to be included
            return boost::starts_with(instance_id, L"root\\rdp_kbd") || boost::starts_with(instance_id, L"root\\rdp_mou");
        }

        struct hid_device_info {
            RB_COPYABLE(hid_device_info);

            bool m_has_attrs;
            HIDD_ATTRIBUTES m_attrs;
            std::wstring m_serial;
            std::wstring m_product_name;
        };

        hid_device_info get_hid_info(context *ctx, HDEVINFO info_set, SP_DEVINFO_DATA& device, const std::wstring& instance_id, GUID& hid_iface) {
            SP_DEVICE_INTERFACE_DATA iface{};
            iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            DWORD interface_index = 0;

            while (SetupDiEnumDeviceInterfaces(info_set, &device, &hid_iface, interface_index, &iface)) {
                DWORD size = 0;
                if (!SetupDiGetDeviceInterfaceDetail(info_set, &iface, nullptr, 0, &size, nullptr)) {
                    auto error = GetLastError();
                    if (error != ERROR_INSUFFICIENT_BUFFER) {
                        throw_winapi_error(error, "Failed to get size for interface detail data for device %s (iface %d)", to_utf8(instance_id), interface_index);
                    }
                }

                std::vector<char> buffer;
                buffer.resize(size);

                auto buffer_ptr = new (&buffer[0]) SP_DEVICE_INTERFACE_DETAIL_DATA{};
                buffer_ptr->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

                if (!SetupDiGetDeviceInterfaceDetail(info_set, &iface, buffer_ptr, size, nullptr, &device)) {
                    throw_winapi_error("Failed to get interface detail data for device %s (iface %d)", to_utf8(instance_id), interface_index);
                }

                std::wstring path{ buffer_ptr->DevicePath };

                auto handle = CreateFile(
                    path.c_str(), MAXIMUM_ALLOWED,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
                );

                if (handle == INVALID_HANDLE_VALUE) {
                    ctx->log_verbose("setupdi: [%s] cannot open interface %s (#%d): %s",
                        to_utf8(instance_id), to_utf8(path), interface_index, describe_winapi_error()
                    );
                    interface_index++;
                    continue;
                }

                std::unique_ptr<void, decltype(&CloseHandle)> defer_handle{ handle, CloseHandle };

                hid_device_info info{};
                info.m_attrs.Size = sizeof(HIDD_ATTRIBUTES);

                if (!HidD_GetAttributes(handle, &info.m_attrs)) {
                    ctx->log_verbose("setupdi: [%s] cannot fetch HID attributes from iface %s (#%d): %s",
                        to_utf8(instance_id), to_utf8(path), interface_index, describe_winapi_error()
                    );
                    interface_index++;
                    continue;
                }

                info.m_has_attrs = true;

                std::vector<wchar_t> str_buffer{};
                str_buffer.resize(126);

                if (HidD_GetSerialNumberString(handle, &str_buffer[0], str_buffer.size() * sizeof(wchar_t))) {
                    info.m_serial = std::wstring{ &str_buffer[0] };
                } else {
                    ctx->log_verbose("setupdi: [%s] cannot fetch HID serial number from iface %s (#%d): %s",
                        to_utf8(instance_id), to_utf8(path), interface_index, describe_winapi_error()
                    );
                }

                str_buffer.clear();
                str_buffer.resize(126);

                if (HidD_GetProductString(handle, &str_buffer[0], str_buffer.size() * sizeof(wchar_t))) {
                    std::wstring tmp_name{ &str_buffer[0] };
                    if (!boost::iequals(tmp_name, L"USB Device")) {
                        // XXX hacky, probably won't work well with i18n
                        info.m_product_name = tmp_name;
                    }
                } else {
                    ctx->log_verbose("setupdi: [%s] cannot fetch HID product name from iface %s (#%d): %s",
                        to_utf8(instance_id), to_utf8(path), interface_index, describe_winapi_error()
                    );
                }

                return info;
            }

            return {};
        }

        std::unordered_map<std::wstring, setup_device_info> enumerate_devices(context* ctx) {
            ctx->log_verbose("setupdi: enumerating devices");

            auto info_set = SetupDiGetClassDevs(nullptr, nullptr, nullptr, DIGCF_ALLCLASSES | DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
            if (info_set == INVALID_HANDLE_VALUE) {
                throw_winapi_error("Failed to enumerate present devices");
            }

            std::unique_ptr<void, decltype(&SetupDiDestroyDeviceInfoList)> defer_info_set{ info_set, SetupDiDestroyDeviceInfoList };

            SP_DEVINFO_DATA device{};
            device.cbSize = sizeof(SP_DEVINFO_DATA);
            DWORD device_index = 0;

            GUID hid_iface{};
            HidD_GetHidGuid(&hid_iface);

            std::unordered_map<std::wstring, setup_device_info> result{};

            while (true) {
                if (!SetupDiEnumDeviceInfo(info_set, device_index, &device)) {
                    auto error = GetLastError();

                    if (error != ERROR_NO_MORE_ITEMS) {
                        throw_winapi_error(error, "Failed to query device information for index %1%", device_index);
                    }

                    break;
                }

                auto instance_id = get_device_instance_id(info_set, device);
                auto name = get_device_friendly_name(info_set, device, instance_id);
                auto location = instance_id;
                setup_device_info device_info{};

                if (name.empty()) {
                    ctx->log_verbose("setupdi: %s has no name", to_utf8(instance_id));
                    continue;
                } else if (is_blacklisted(name)) {
                    ctx->log_verbose("setupdi: %s is blacklisted", to_utf8(instance_id));
                    continue;
                } else {
                    ctx->log_verbose("setupdi: found %s (%s)", to_utf8(name), to_utf8(instance_id));
                }

                if (result.find(instance_id) == result.end()) {
                    auto hid_info = get_hid_info(ctx, info_set, device, instance_id, hid_iface);

                    if (hid_info.m_has_attrs) {
                        if (hid_info.m_product_name.size() > 0) {
                            name = hid_info.m_product_name;
                        }

                        device_info.m_vendor_id = hid_info.m_attrs.VendorID;
                        device_info.m_product_id = hid_info.m_attrs.ProductID;
                        device_info.m_revision = hid_info.m_attrs.VersionNumber;
                        device_info.m_serial = to_utf8(hid_info.m_serial);

                        ctx->log_verbose("setupdi: [%s] adding %s (VID = %x PID = %x Rev = %x SN = '%s')",
                            to_utf8(instance_id), to_utf8(name),
                            hid_info.m_attrs.VendorID, hid_info.m_attrs.ProductID,
                            hid_info.m_attrs.VersionNumber, to_utf8(hid_info.m_serial)
                        );
                    } else {
                        ctx->log_verbose("setupdi: [%s] adding %s (non-HID)",
                            to_utf8(instance_id), to_utf8(name)
                        );
                    }

                    device_info.m_instance_id = to_utf8(instance_id);
                    device_info.m_friendly_name = to_utf8(name);
                    device_info.m_location_id = to_utf8(location);

                    result[instance_id] = device_info;
                } else {
                    ctx->log_verbose("setupdi: [%s] duplicate device", to_utf8(instance_id));
                }

                ++device_index;
            }

            return result;
        }
    }
}
