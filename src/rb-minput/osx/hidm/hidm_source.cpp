// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include <memory>
#include <unordered_set>
#include <unordered_map>

#include "osx/osx.hpp"
#include "osx/hidm/hidm_source.hpp"
#include "osx/hidm/hidm_device.hpp"
#include "device.hpp"
#include "context.hpp"
#include "input_code.hpp"
#include "axis_utils.hpp"

#include <ForceFeedback/ForceFeedback.h>
#include <IOKit/hid/IOHIDManager.h>

#include <CF++.hpp>

#define RB_OSX_ENUMERATE_MODE CFSTR("RBMultiInputDeviceEnumerate")

namespace multi_input {
    namespace osx {
        hid_manager::hid_manager(context* ctx) :
            m_ctx(ctx),
            m_ref(IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDManagerOptionNone)),
            m_open(false),
            m_scheduled(false)
        {
            if (m_ref == nullptr) {
                throw_osx_error("Failed to create HIDManager instance");
            }
        }

        hid_manager::~hid_manager() {
            close();
            CFRelease(m_ref);
        }

        void hid_manager::open() {
            if (!m_open) {
                auto code = IOHIDManagerOpen(m_ref, kIOHIDManagerOptionNone);
                if (code != kIOReturnSuccess) {
                    throw_osx_error(code, "Failed to open HIDManager");
                }
                m_open = true;
            }
        }

        void hid_manager::close() {
            if (m_open) {
                unschedule();
                IOHIDManagerClose(m_ref, kIOHIDManagerOptionNone);
                m_open = false;
            }
        }

        void hid_manager::reopen() {
            close();
            open();
        }

        void hid_manager::schedule() {
            if (!m_open) {
                m_ctx->log_warning("Trying to schedule unopened HIDManager");
            }

            if (!m_scheduled) {
                IOHIDManagerScheduleWithRunLoop(m_ref, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                m_scheduled = true;
            }
        }

        void hid_manager::unschedule() {
            if (!m_open) {
                m_ctx->log_warning("Trying to unschedule unopened HIDManager");
            }

            if (m_scheduled) {
                IOHIDManagerUnscheduleFromRunLoop(m_ref, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                m_scheduled = false;
            }
        }

        CF::Dictionary match_usage(uint32_t usage_page, uint32_t usage) {
            CF::Dictionary dict{};
            dict << CF::Pair{ kIOHIDDeviceUsagePageKey, CF::Number{ usage } };
            dict << CF::Pair{ kIOHIDDeviceUsageKey, CF::Number{ usage_page } };
            return dict;
        }

        hidm_source::hidm_source(context* ctx) :
            source(ctx), m_hid_manager{ctx}, m_device_map{}
        {
            RB_TRACE_ENTER();

            RB_TRACE("configuring HIDManager");
            IOHIDManagerSetDeviceMatchingMultiple(m_hid_manager.get(), nullptr);

            IOHIDManagerRegisterDeviceMatchingCallback(m_hid_manager.get(), [](void *self, IOReturn result, void*, IOHIDDeviceRef device) {
                if (result != kIOReturnSuccess) return;
                ((hidm_source*)self)->add_device(device);
            }, this);

            IOHIDManagerRegisterDeviceRemovalCallback(m_hid_manager.get(), [](void *self, IOReturn result, void*, IOHIDDeviceRef device) {
                if (result != kIOReturnSuccess) return;
                ((hidm_source*)self)->remove_device(device);
            }, this);
        }

        hidm_source::~hidm_source() {
            m_hid_manager.unschedule();
            IOHIDManagerRegisterDeviceMatchingCallback(m_hid_manager.get(), nullptr, nullptr);
            IOHIDManagerRegisterDeviceRemovalCallback(m_hid_manager.get(), nullptr, nullptr);
        }

        void hidm_source::enum_devices() {
            RB_TRACE_ENTER();

            RB_TRACE("discarding devices");
            for (auto&& pair : m_device_map) {
                m_ctx->remove_device(pair.second);
            }

            m_device_map.clear();

            RB_TRACE("reopening HIDManager");
            m_ctx->log_verbose("hidm: (re)opening manager");
            m_hid_manager.reopen();

            RB_TRACE("enumerating devices");
            auto device_set = IOHIDManagerCopyDevices(m_hid_manager.get());

            if (device_set == nullptr) {
                m_ctx->log_info("hidm: no devices found");
            } else {
                auto device_count = CFSetGetCount(device_set);
                m_ctx->log_info("hidm: %1% devices found", device_count);

                std::vector<IOHIDDeviceRef> devices{};
                devices.resize(device_count);
                CFSetGetValues(device_set, (const void**)&devices[0]);
                CFRelease(device_set);

                for (auto&& device : devices) {
                    add_device(device);
                }
            }

            RB_TRACE("scheduling HIDManager");
            m_ctx->log_verbose("hidm: scheduling for regular operation");
            m_hid_manager.schedule();
        }

        void hidm_source::add_device(IOHIDDeviceRef handle) {
            RB_TRACE_ENTER();

            if (m_device_map.find(handle) != m_device_map.end()) {
                RB_TRACE("skipping device: already exists");
                return;
            }

            std::string name = CF::String{ IOHIDDeviceGetProperty(handle, CFSTR(kIOHIDProductKey)) };

            m_ctx->log_verbose("hidm: found device %p: %s", handle, name);

            if (name.empty()) {
                RB_TRACE("skipping device: no name");
                return;
            }

            RB_TRACE("creating new device object");
            auto id = m_ctx->get_next_id();
            m_ctx->add_device(std::unique_ptr<device>{new hidm_device(m_ctx, id, name, handle)});
            m_device_map.emplace(handle, id);
        }

        void hidm_source::remove_device(IOHIDDeviceRef handle) {
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

        hidm_device* hidm_source::get_device(IOHIDDeviceRef handle) {
            RB_TRACE_ENTER();

            auto it = m_device_map.find(handle);
            if (it == m_device_map.end()) {
                RB_TRACE("unknown device: not in m_device_map");
                return nullptr;
            }

            auto device_ptr = static_cast<hidm_device*>(m_ctx->get_device(it->second));
            if (device_ptr == nullptr) {
                RB_TRACE("unknown device: not in context devices");
                return nullptr;
            }

            return device_ptr;
        }

        void hidm_source::drain_events() {
        }
    }
}
