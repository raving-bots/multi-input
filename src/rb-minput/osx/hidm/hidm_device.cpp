// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "osx/hidm/hidm_device.hpp"
#include "osx/hid_debug.hpp"
#include "device.hpp"
#include "context.hpp"
#include "input_code.hpp"
#include "axis_utils.hpp"

#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/usb/IOUSBLib.h>
#include <ForceFeedback/ForceFeedback.h>

#include <CF++.hpp>

namespace multi_input {
    namespace osx {
        hidm_device::hidm_device(context* ctx, device_id id, std::string name, IOHIDDeviceRef handle) :
            device(ctx, id), m_handle(handle), m_axis_map(), m_feedback_handle(),
            m_last_effect(), m_feedback_caps(), m_can_vibrate()
        {
            CFRetain(m_handle);

            int location = CF::Number{ IOHIDDeviceGetProperty(m_handle, CFSTR(kIOHIDLocationIDKey)) };
            int unique = CF::Number{ IOHIDDeviceGetProperty(m_handle, CFSTR(kIOHIDUniqueIDKey)) };
            int vendor = CF::Number{ IOHIDDeviceGetProperty(m_handle, CFSTR(kIOHIDVendorIDKey)) };
            int product = CF::Number{ IOHIDDeviceGetProperty(m_handle, CFSTR(kIOHIDProductIDKey)) };
            int version = CF::Number{ IOHIDDeviceGetProperty(m_handle, CFSTR(kIOHIDVersionNumberKey)) };
            std::string serial = CF::String{ IOHIDDeviceGetProperty(m_handle, CFSTR(kIOHIDSerialNumberKey)) };

            auto& meta = get_meta();
            meta.set_name(name);
            meta.set_location(std::to_string(location));
            meta.set_internal_id(std::to_string(unique));
            meta.set_ids(vendor, product, version);
            meta.set_serial(serial);

            CF::Array top_elements = IOHIDDeviceGetProperty(m_handle, CFSTR(kIOHIDElementKey));
            std::string description{};
            int ignored{};

            for (auto element_dict : top_elements) {
                auto cookie = (CFTypeRef)CFDictionaryGetValue((CFDictionaryRef)element_dict, CFSTR(kIOHIDElementCookieKey));

                CF::Dictionary match{};
                match << CF::Pair{ kIOHIDElementCookieKey, cookie };

                auto match_result_ref = IOHIDDeviceCopyMatchingElements(m_handle, match, 0);
                CF::Array match_result = match_result_ref;
                CFRelease(match_result_ref);

                auto element = (IOHIDElementRef)match_result[0];
                auto ignore_me = true;

                if (IOHIDElementGetType(element) == kIOHIDElementTypeCollection) {
                    if (IOHIDElementGetCollectionType(element) == kIOHIDElementCollectionTypeApplication) {
                        auto page = IOHIDElementGetUsagePage(element);
                        auto usage = IOHIDElementGetUsage(element);
                        add_element(element, page, usage);
                        ignore_me = false;
                    }
                }

                description += describe_element(element, 1, ignore_me);
                if (ignore_me) {
                    ignored++;
                }
            }

            description = format("device %|x| (%|d| top-level elements, %|d| ignored):\n", m_handle, top_elements.GetCount(), ignored) + description;
            //m_ctx->log_verbose("hidm: %1%", description);

            IOHIDDeviceRegisterInputValueCallback(m_handle, [](void *self, IOReturn result, void*, IOHIDValueRef value) {
                if (result != kIOReturnSuccess) return;
                ((hidm_device*)self)->on_input(value);
            }, this);

            auto io_service = IOHIDDeviceGetService(m_handle);
            if (io_service == MACH_PORT_NULL) {
                m_ctx->log_verbose("hidm: device %|x| has no io_service_t", m_handle);
            } else {
                //if (get_meta().get_serial().empty()) {
                //    CFMutableDictionaryRef props = nullptr;
                //    if (IORegistryEntryCreateCFProperties(io_service, &props, kCFAllocatorDefault, kNilOptions) == KERN_SUCCESS) {
                //        CF::Dictionary dict{props};
                //        CF::String usb_serial = dict[kUSBSerialNumberString];
                //        if (usb_serial.IsValid()) {
                //            get_meta().set_serial(std::string{usb_serial});
                //        }
                //    }
                //}

                auto error = FFCreateDevice(io_service, &m_feedback_handle);
                if (error != FF_OK) {
                    //m_ctx->log_verbose("hidm: FFCreateDevice failed for device %|x| (io_service_t %|x|)", m_handle, io_service);
                    m_feedback_handle = nullptr;
                } else {
                    if (FFDeviceGetForceFeedbackCapabilities(m_feedback_handle, &m_feedback_caps) == FF_OK &&
                        (m_feedback_caps.supportedEffects & FFCAP_ET_CUSTOMFORCE) != 0 &&
                        m_feedback_caps.numFfAxes == 2)
                    {
                        m_can_vibrate = true;
                    }
                }
            }

            if (m_axis_map.empty()) {
                m_is_usable = false;
            }
        }

        hidm_device::~hidm_device() {
            if (m_feedback_handle != nullptr) {
                if (m_last_effect != nullptr) {
                    FFDeviceReleaseEffect(m_feedback_handle, m_last_effect);
                }

                FFReleaseDevice(m_feedback_handle);
            }

            IOHIDDeviceRegisterInputValueCallback(m_handle, nullptr, nullptr);
            CFRelease(m_handle);
        }

        void hidm_device::add_axis(IOHIDElementRef element, input_code axis) {
            auto cookie = IOHIDElementGetCookie(element);
            m_axis_map[cookie] = axis;
            device::add_axis(axis);
        }

        void hidm_device::add_element(IOHIDElementRef element, uint32_t collection_page, uint32_t collection_id) {
            switch (IOHIDElementGetType(element)) {
                case kIOHIDElementTypeCollection: {
                    CF::Array children = IOHIDElementGetChildren(element);
                    for (auto child : children) {
                        add_element((IOHIDElementRef)child, collection_page, collection_id);
                    }
                    break;
                }
                case kIOHIDElementTypeInput_Misc:
                case kIOHIDElementTypeInput_Axis:
                case kIOHIDElementTypeInput_Button:
                case kIOHIDElementTypeInput_ScanCodes:
                    switch (collection_page) {
                        case kHIDPage_GenericDesktop:
                            switch (collection_id) {
                                case kHIDUsage_GD_Pointer:
                                case kHIDUsage_GD_Mouse:
                                    add_mouse_element(element);
                                break;

                                case kHIDUsage_GD_Keypad:
                                case kHIDUsage_GD_Keyboard:
                                    add_keyboard_element(element);
                                break;

                                case kHIDUsage_GD_Joystick:
                                case kHIDUsage_GD_GamePad:
                                case kHIDUsage_GD_MultiAxisController:
                                    add_gamepad_element(element);
                                break;
                            }
                        break;
                    }
                break;
                default:
                break;
            }
        }

        void hidm_device::add_mouse_element(IOHIDElementRef element) {
            auto page = IOHIDElementGetUsagePage(element);
            auto usage = IOHIDElementGetUsage(element);

            switch (page) {
                case kHIDPage_GenericDesktop:
                    switch (usage) {
                        case kHIDUsage_GD_X:
                            add_axis(element, input_code::mouse_x);
                            device::add_axis(input_code::mouse_x_left);
                            device::add_axis(input_code::mouse_x_right);
                        break;
                        case kHIDUsage_GD_Y:
                            add_axis(element, input_code::mouse_y);
                            device::add_axis(input_code::mouse_y_up);
                            device::add_axis(input_code::mouse_y_down);
                        break;
                        case kHIDUsage_GD_Wheel:
                            add_axis(element, input_code::mouse_wheel);
                            device::add_axis(input_code::mouse_wheel_up);
                            device::add_axis(input_code::mouse_wheel_down);
                        break;
                    }
                break;
                case kHIDPage_Button: {
                    if (usage > 0 && usage < 8) {
                        auto button = usage - 1;
                        auto axis = static_cast<int>(input_code::mouse_left) + button;
                        add_axis(element, static_cast<input_code>(axis));
                    }
                    break;
                }
            }
        }

        void hidm_device::add_keyboard_element(IOHIDElementRef element) {
            auto page = IOHIDElementGetUsagePage(element);
            auto usage = IOHIDElementGetUsage(element);

            if (page != kHIDPage_KeyboardOrKeypad) {
                return;
            }

            switch (usage) {
                case kHIDUsage_KeyboardA:
                    add_axis(element, input_code::key_a);
                break;
                case kHIDUsage_KeyboardB:
                    add_axis(element, input_code::key_b);
                break;
                case kHIDUsage_KeyboardC:
                    add_axis(element, input_code::key_c);
                break;
                case kHIDUsage_KeyboardD:
                    add_axis(element, input_code::key_d);
                break;
                case kHIDUsage_KeyboardE:
                    add_axis(element, input_code::key_e);
                break;
                case kHIDUsage_KeyboardF:
                    add_axis(element, input_code::key_f);
                break;
                case kHIDUsage_KeyboardG:
                    add_axis(element, input_code::key_g);
                break;
                case kHIDUsage_KeyboardH:
                    add_axis(element, input_code::key_h);
                break;
                case kHIDUsage_KeyboardI:
                    add_axis(element, input_code::key_i);
                break;
                case kHIDUsage_KeyboardJ:
                    add_axis(element, input_code::key_j);
                break;
                case kHIDUsage_KeyboardK:
                    add_axis(element, input_code::key_k);
                break;
                case kHIDUsage_KeyboardL:
                    add_axis(element, input_code::key_l);
                break;
                case kHIDUsage_KeyboardM:
                    add_axis(element, input_code::key_m);
                break;
                case kHIDUsage_KeyboardN:
                    add_axis(element, input_code::key_n);
                break;
                case kHIDUsage_KeyboardO:
                    add_axis(element, input_code::key_o);
                break;
                case kHIDUsage_KeyboardP:
                    add_axis(element, input_code::key_p);
                break;
                case kHIDUsage_KeyboardQ:
                    add_axis(element, input_code::key_q);
                break;
                case kHIDUsage_KeyboardR:
                    add_axis(element, input_code::key_r);
                break;
                case kHIDUsage_KeyboardS:
                    add_axis(element, input_code::key_s);
                break;
                case kHIDUsage_KeyboardT:
                    add_axis(element, input_code::key_t);
                break;
                case kHIDUsage_KeyboardU:
                    add_axis(element, input_code::key_u);
                break;
                case kHIDUsage_KeyboardV:
                    add_axis(element, input_code::key_v);
                break;
                case kHIDUsage_KeyboardW:
                    add_axis(element, input_code::key_w);
                break;
                case kHIDUsage_KeyboardX:
                    add_axis(element, input_code::key_x);
                break;
                case kHIDUsage_KeyboardY:
                    add_axis(element, input_code::key_y);
                break;
                case kHIDUsage_KeyboardZ:
                    add_axis(element, input_code::key_z);
                break;
                case kHIDUsage_Keyboard1:
                    add_axis(element, input_code::key_1);
                break;
                case kHIDUsage_Keyboard2:
                    add_axis(element, input_code::key_2);
                break;
                case kHIDUsage_Keyboard3:
                    add_axis(element, input_code::key_3);
                break;
                case kHIDUsage_Keyboard4:
                    add_axis(element, input_code::key_4);
                break;
                case kHIDUsage_Keyboard5:
                    add_axis(element, input_code::key_5);
                break;
                case kHIDUsage_Keyboard6:
                    add_axis(element, input_code::key_6);
                break;
                case kHIDUsage_Keyboard7:
                    add_axis(element, input_code::key_7);
                break;
                case kHIDUsage_Keyboard8:
                    add_axis(element, input_code::key_8);
                break;
                case kHIDUsage_Keyboard9:
                    add_axis(element, input_code::key_9);
                break;
                case kHIDUsage_Keyboard0:
                    add_axis(element, input_code::key_0);
                break;
                case kHIDUsage_KeyboardReturnOrEnter:
                    add_axis(element, input_code::key_enter);
                break;
                case kHIDUsage_KeyboardEscape:
                    add_axis(element, input_code::key_escape);
                break;
                case kHIDUsage_KeyboardDeleteOrBackspace:
                    add_axis(element, input_code::key_backspace);
                break;
                case kHIDUsage_KeyboardTab:
                    add_axis(element, input_code::key_tab);
                break;
                case kHIDUsage_KeyboardSpacebar:
                    add_axis(element, input_code::key_space);
                break;
                case kHIDUsage_KeyboardHyphen:
                    add_axis(element, input_code::key_minus);
                break;
                case kHIDUsage_KeyboardEqualSign:
                    add_axis(element, input_code::key_plus);
                break;
                case kHIDUsage_KeyboardOpenBracket:
                    add_axis(element, input_code::key_left_bracket);
                break;
                case kHIDUsage_KeyboardCloseBracket:
                    add_axis(element, input_code::key_right_bracket);
                break;
                case kHIDUsage_KeyboardBackslash:
                    add_axis(element, input_code::key_backslash);
                break;
                case kHIDUsage_KeyboardNonUSPound:
                    add_axis(element, input_code::key_backslash);
                break;
                case kHIDUsage_KeyboardSemicolon:
                    add_axis(element, input_code::key_semicolon);
                break;
                case kHIDUsage_KeyboardQuote:
                    add_axis(element, input_code::key_quote);
                break;
                case kHIDUsage_KeyboardGraveAccentAndTilde:
                    add_axis(element, input_code::key_accent);
                break;
                case kHIDUsage_KeyboardComma:
                    add_axis(element, input_code::key_comma);
                break;
                case kHIDUsage_KeyboardPeriod:
                    add_axis(element, input_code::key_period);
                break;
                case kHIDUsage_KeyboardSlash:
                    add_axis(element, input_code::key_slash);
                break;
                case kHIDUsage_KeyboardLockingCapsLock:
                case kHIDUsage_KeyboardCapsLock:
                    add_axis(element, input_code::key_caps_lock);
                break;
                case kHIDUsage_KeyboardF1:
                    add_axis(element, input_code::key_f1);
                break;
                case kHIDUsage_KeyboardF2:
                    add_axis(element, input_code::key_f2);
                break;
                case kHIDUsage_KeyboardF3:
                    add_axis(element, input_code::key_f3);
                break;
                case kHIDUsage_KeyboardF4:
                    add_axis(element, input_code::key_f4);
                break;
                case kHIDUsage_KeyboardF5:
                    add_axis(element, input_code::key_f5);
                break;
                case kHIDUsage_KeyboardF6:
                    add_axis(element, input_code::key_f6);
                break;
                case kHIDUsage_KeyboardF7:
                    add_axis(element, input_code::key_f7);
                break;
                case kHIDUsage_KeyboardF8:
                    add_axis(element, input_code::key_f8);
                break;
                case kHIDUsage_KeyboardF9:
                    add_axis(element, input_code::key_f9);
                break;
                case kHIDUsage_KeyboardF10:
                    add_axis(element, input_code::key_f10);
                break;
                case kHIDUsage_KeyboardF11:
                    add_axis(element, input_code::key_f11);
                break;
                case kHIDUsage_KeyboardF12:
                    add_axis(element, input_code::key_f12);
                break;
                case kHIDUsage_KeyboardPrintScreen:
                    add_axis(element, input_code::key_print_screen);
                break;
                case kHIDUsage_KeyboardLockingScrollLock:
                case kHIDUsage_KeyboardScrollLock:
                    add_axis(element, input_code::key_scroll_lock);
                break;
                case kHIDUsage_KeyboardPause:
                    add_axis(element, input_code::key_pause);
                break;
                case kHIDUsage_KeyboardInsert:
                    add_axis(element, input_code::key_insert);
                break;
                case kHIDUsage_KeyboardHome:
                    add_axis(element, input_code::key_home);
                break;
                case kHIDUsage_KeyboardPageUp:
                    add_axis(element, input_code::key_page_up);
                break;
                case kHIDUsage_KeyboardDeleteForward:
                    add_axis(element, input_code::key_delete);
                break;
                case kHIDUsage_KeyboardEnd:
                    add_axis(element, input_code::key_end);
                break;
                case kHIDUsage_KeyboardPageDown:
                    add_axis(element, input_code::key_page_down);
                break;
                case kHIDUsage_KeyboardRightArrow:
                    add_axis(element, input_code::key_right_arrow);
                break;
                case kHIDUsage_KeyboardLeftArrow:
                    add_axis(element, input_code::key_left_arrow);
                break;
                case kHIDUsage_KeyboardDownArrow:
                    add_axis(element, input_code::key_down_arrow);
                break;
                case kHIDUsage_KeyboardUpArrow:
                    add_axis(element, input_code::key_up_arrow);
                break;
                case kHIDUsage_KeyboardLockingNumLock:
                case kHIDUsage_KeypadNumLock:
                    add_axis(element, input_code::key_num_lock);
                break;
                case kHIDUsage_KeypadSlash:
                    add_axis(element, input_code::key_num_divide);
                break;
                case kHIDUsage_KeypadAsterisk:
                    add_axis(element, input_code::key_num_multiply);
                break;
                case kHIDUsage_KeypadHyphen:
                    add_axis(element, input_code::key_num_minus);
                break;
                case kHIDUsage_KeypadPlus:
                    add_axis(element, input_code::key_num_plus);
                break;
                case kHIDUsage_KeypadEnter:
                    add_axis(element, input_code::key_num_enter);
                break;
                case kHIDUsage_Keypad1:
                    add_axis(element, input_code::key_num_1);
                break;
                case kHIDUsage_Keypad2:
                    add_axis(element, input_code::key_num_2);
                break;
                case kHIDUsage_Keypad3:
                    add_axis(element, input_code::key_num_3);
                break;
                case kHIDUsage_Keypad4:
                    add_axis(element, input_code::key_num_4);
                break;
                case kHIDUsage_Keypad5:
                    add_axis(element, input_code::key_num_5);
                break;
                case kHIDUsage_Keypad6:
                    add_axis(element, input_code::key_num_6);
                break;
                case kHIDUsage_Keypad7:
                    add_axis(element, input_code::key_num_7);
                break;
                case kHIDUsage_Keypad8:
                    add_axis(element, input_code::key_num_8);
                break;
                case kHIDUsage_Keypad9:
                    add_axis(element, input_code::key_num_9);
                break;
                case kHIDUsage_Keypad0:
                    add_axis(element, input_code::key_num_0);
                break;
                case kHIDUsage_KeypadPeriod:
                    add_axis(element, input_code::key_num_decimal);
                break;
                case kHIDUsage_KeyboardNonUSBackslash:
                    add_axis(element, input_code::key_oem_102);
                break;
                case kHIDUsage_KeyboardApplication:
                    add_axis(element, input_code::key_apps);
                break;
                case kHIDUsage_KeypadEqualSign:
                    add_axis(element, input_code::key_plus);
                break;
                case kHIDUsage_KeyboardF13:
                    add_axis(element, input_code::key_f13);
                break;
                case kHIDUsage_KeyboardF14:
                    add_axis(element, input_code::key_f14);
                break;
                case kHIDUsage_KeyboardF15:
                    add_axis(element, input_code::key_f15);
                break;
                case kHIDUsage_KeyboardF16:
                    add_axis(element, input_code::key_f16);
                break;
                case kHIDUsage_KeyboardF17:
                    add_axis(element, input_code::key_f17);
                break;
                case kHIDUsage_KeyboardF18:
                    add_axis(element, input_code::key_f18);
                break;
                case kHIDUsage_KeyboardF19:
                    add_axis(element, input_code::key_f19);
                break;
                case kHIDUsage_KeyboardF20:
                    add_axis(element, input_code::key_f20);
                break;
                case kHIDUsage_KeyboardF21:
                    add_axis(element, input_code::key_f21);
                break;
                case kHIDUsage_KeyboardF22:
                    add_axis(element, input_code::key_f22);
                break;
                case kHIDUsage_KeyboardF23:
                    add_axis(element, input_code::key_f23);
                break;
                case kHIDUsage_KeyboardF24:
                    add_axis(element, input_code::key_f24);
                break;
                case kHIDUsage_KeypadComma:
                    add_axis(element, input_code::key_comma);
                break;
                case kHIDUsage_KeypadEqualSignAS400:
                    add_axis(element, input_code::key_plus);
                break;
                case kHIDUsage_KeyboardInternational1:
                    add_axis(element, input_code::key_slash);
                break;
                case kHIDUsage_KeyboardClear:
                    add_axis(element, input_code::key_clear);
                break;
                case kHIDUsage_KeyboardLeftControl:
                    add_axis(element, input_code::key_left_control);
                break;
                case kHIDUsage_KeyboardLeftShift:
                    add_axis(element, input_code::key_left_shift);
                break;
                case kHIDUsage_KeyboardLeftAlt:
                    add_axis(element, input_code::key_left_alt);
                break;
                case kHIDUsage_KeyboardLeftGUI:
                    add_axis(element, input_code::key_left_command);
                break;
                case kHIDUsage_KeyboardRightControl:
                    add_axis(element, input_code::key_right_control);
                break;
                case kHIDUsage_KeyboardRightShift:
                    add_axis(element, input_code::key_right_shift);
                break;
                case kHIDUsage_KeyboardRightAlt:
                    add_axis(element, input_code::key_right_alt);
                break;
                case kHIDUsage_KeyboardRightGUI:
                    add_axis(element, input_code::key_right_command);
                break;
            }
        }

        void hidm_device::add_gamepad_element(IOHIDElementRef element) {
            auto page = IOHIDElementGetUsagePage(element);
            auto usage = IOHIDElementGetUsage(element);

            // TODO non-xbox

            switch (page) {
                case kHIDPage_GenericDesktop:
                    switch (usage) {
                        case kHIDUsage_GD_X:
                            add_axis(element, input_code::pad_left_stick_x);
                            device::add_axis(input_code::pad_left_stick_left);
                            device::add_axis(input_code::pad_left_stick_right);
                        break;
                        case kHIDUsage_GD_Y:
                            add_axis(element, input_code::pad_left_stick_y);
                            device::add_axis(input_code::pad_left_stick_up);
                            device::add_axis(input_code::pad_left_stick_down);
                        break;
                        case kHIDUsage_GD_Z:
                            add_axis(element, input_code::pad_left_trigger);
                        break;
                        case kHIDUsage_GD_Rx:
                            add_axis(element, input_code::pad_right_stick_x);
                            device::add_axis(input_code::pad_right_stick_left);
                            device::add_axis(input_code::pad_right_stick_right);
                        break;
                        case kHIDUsage_GD_Ry:
                            add_axis(element, input_code::pad_right_stick_y);
                            device::add_axis(input_code::pad_right_stick_up);
                            device::add_axis(input_code::pad_right_stick_down);
                        break;
                        case kHIDUsage_GD_Rz:
                            add_axis(element, input_code::pad_right_trigger);
                        break;
                    }
                case kHIDPage_Button:
                    switch (usage) {
                        case 1:
                            add_axis(element, input_code::pad_a);
                        break;
                        case 2:
                            add_axis(element, input_code::pad_b);
                        break;
                        case 3:
                            add_axis(element, input_code::pad_x);
                        break;
                        case 4:
                            add_axis(element, input_code::pad_y);
                        break;
                        case 5:
                            add_axis(element, input_code::pad_left_bumper);
                        break;
                        case 6:
                            add_axis(element, input_code::pad_right_bumper);
                        break;
                        case 7:
                            add_axis(element, input_code::pad_left_stick);
                        break;
                        case 8:
                            add_axis(element, input_code::pad_right_stick);
                        break;
                        case 9:
                            add_axis(element, input_code::pad_start);
                        break;
                        case 10:
                            add_axis(element, input_code::pad_back);
                        break;
                        case 12:
                            add_axis(element, input_code::pad_dpad_up);
                            device::add_axis(input_code::pad_dpad_y);
                        break;
                        case 13:
                            add_axis(element, input_code::pad_dpad_down);
                            device::add_axis(input_code::pad_dpad_y);
                        break;
                        case 14:
                            add_axis(element, input_code::pad_dpad_left);
                            device::add_axis(input_code::pad_dpad_x);
                        break;
                        case 15:
                            add_axis(element, input_code::pad_dpad_right);
                            device::add_axis(input_code::pad_dpad_x);
                        break;
                    }
            }
        }

        // TODO non-xbox, unify platforms

        float get_pad_deadzone_for(input_code code) {
            switch (code) {
                case input_code::pad_left_stick_x:
                case input_code::pad_left_stick_y:
                    return deadzone_left_stick;
                case input_code::pad_right_stick_x:
                case input_code::pad_right_stick_y:
                    return deadzone_right_stick;
                case input_code::pad_left_trigger:
                case input_code::pad_right_trigger:
                    return deadzone_trigger;
                default:
                    return 0;
            }
        }

        float get_pad_max_for(input_code code) {
            switch (code) {
                case input_code::pad_left_stick_x:
                case input_code::pad_left_stick_y:
                case input_code::pad_right_stick_x:
                case input_code::pad_right_stick_y:
                    return 32767.0f;
                case input_code::pad_left_trigger:
                case input_code::pad_right_trigger:
                    return 255.0f;
                default:
                    return 0;
            }
        }

        float map_pad_value(input_code code, float raw_value) {
            auto deadzone = get_pad_deadzone_for(code);
            auto max = get_pad_max_for(code);

            if (code == input_code::pad_left_stick_y || code == input_code::pad_right_stick_y) {
                // reverse Y (we're doing Y+Up)
                raw_value = -raw_value;
            }

            if (raw_value < 0) {
                return -apply_deadzone(-raw_value, max, deadzone);
            } else {
                return apply_deadzone(raw_value, max, deadzone);
            }
        }

        void hidm_device::on_input(IOHIDValueRef value_ref) {
            auto element = IOHIDValueGetElement(value_ref);
            auto raw_value = IOHIDValueGetIntegerValue(value_ref);
            auto cookie = IOHIDElementGetCookie(element);
            auto it = m_axis_map.find(cookie);

            if (it == m_axis_map.end()) {
                //m_ctx->log_verbose("hidm: %|x|: input on unmapped element: %|s| == %|d|", m_handle, describe_element(element, 0, false), raw_value);
            } else {
                auto code = it->second;
                auto axis = get_axis(code);
                float value{};

                switch (code) {
                    case input_code::pad_left_stick_x:
                    case input_code::pad_left_stick_y:
                    case input_code::pad_right_stick_x:
                    case input_code::pad_right_stick_y:
                    case input_code::pad_left_trigger:
                    case input_code::pad_right_trigger:
                        value = map_pad_value(code, raw_value);
                        break;
                    default:
                        value = raw_value;
                        break;
                }

                //m_ctx->log_verbose("hidm: %|x|: input on axis %|s| = raw: %|d| value: %|f|", m_handle, to_string(it->second), raw_value, value);
                axis->set(value);
            }
        }

        void hidm_device::commit() {
            derive_stick_pre_commit(*this);
            derive_mouse_pre_commit(*this);
            device::commit();
            derive_mouse_post_commit(*this);
        }

        bool hidm_device::vibrate(int duration, float left, float right) {
            RB_TRACE_ENTER();

            if (!can_vibrate() || !is_usable()) {
                RB_TRACE("cannot vibrate");
                return false;
            }

            m_ctx->log_debug("hidm: vibrating device %1% with force %2%/%3% for %4%ms", m_handle, left, right, duration);

            if (m_last_effect != nullptr) {
                RB_TRACE("removing previous FF effect");
                m_ctx->log_debug("hidm: removing previous FF effect from device memory: %1%", m_last_effect);

                auto error = FFEffectUnload(m_last_effect);
                if (error != FF_OK) {
                    m_ctx->log_error("hidm: device %1%: FFEffectUnload failed for %2% with code %3%", m_handle, m_last_effect, error);
                    return false;
                }

                m_last_effect = nullptr;
            }

            RB_TRACE("creating new FF effect");
            LONG force[] = {
                static_cast<LONG>(left * 16384),
                static_cast<LONG>(right * 65535)
            };
            DWORD axes[] = {
                m_feedback_caps.ffAxes[0],
                m_feedback_caps.ffAxes[1]
            };
            LONG direction[] = { 0l, 0l };

            FFCUSTOMFORCE effect_force{};
            effect_force.cChannels = effect_force.cSamples = 2;
            effect_force.dwSamplePeriod = duration * 1000;
            effect_force.rglForceData = force;

            FFEFFECT effect{};
            effect.dwSize = sizeof(FFEFFECT);
            effect.cAxes = m_feedback_caps.numFfAxes;
            effect.cbTypeSpecificParams = sizeof(FFCUSTOMFORCE);
            effect.dwDuration = duration * 1000;
            effect.dwFlags = FFEFF_CARTESIAN;
            effect.dwGain = 10000;
            effect.dwTriggerButton = FFEB_NOTRIGGER;
            effect.dwTriggerRepeatInterval = FF_INFINITE;
            effect.lpvTypeSpecificParams = &effect_force;
            effect.rgdwAxes = axes;
            effect.rglDirection = direction;

            auto error = FFDeviceCreateEffect(m_feedback_handle, kFFEffectType_CustomForce_ID, &effect, &m_last_effect);
            if (error != FF_OK) {
                m_ctx->log_error("hidm: device %1%: FFDeviceCreateEffect failed with code %2%", m_handle, error);
                m_last_effect = nullptr;
                return false;
            }

            error = FFEffectStart(m_last_effect, 1, FFES_SOLO);
            if (error != FF_OK) {
                m_ctx->log_error("hidm: device %1%: FFEffectStart failed with code %2%", m_handle, error);
                return false;
            }

            return true;
        }
    }
}
