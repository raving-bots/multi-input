// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "windows/winapi.hpp"
#include "windows/raw_input/raw_input_device.hpp"
#include "windows/setupdi.hpp"
#include "context.hpp"
#include "axis_utils.hpp"

namespace multi_input {
    namespace windows {
        raw_input_device::raw_input_device(
            context *ctx, device_id id, void *handle,
            const RID_DEVICE_INFO& info, const setup_device_info& device_info
        ) :
            device(ctx, id), m_handle(handle), m_has_last_input(), m_last_x(), m_last_y()
        {
            auto& meta = get_meta();
            meta.set_name(device_info.m_friendly_name);
            meta.set_location(device_info.m_location_id);
            meta.set_internal_id(device_info.m_instance_id);
            meta.set_ids(device_info.m_vendor_id, device_info.m_product_id, device_info.m_revision);
            meta.set_serial(device_info.m_serial);

            switch (info.dwType) {
                case RIM_TYPEKEYBOARD:
                    add_axis(input_code::key_0);
                    add_axis(input_code::key_1);
                    add_axis(input_code::key_2);
                    add_axis(input_code::key_3);
                    add_axis(input_code::key_4);
                    add_axis(input_code::key_5);
                    add_axis(input_code::key_6);
                    add_axis(input_code::key_7);
                    add_axis(input_code::key_8);
                    add_axis(input_code::key_9);
                    add_axis(input_code::key_a);
                    add_axis(input_code::key_b);
                    add_axis(input_code::key_c);
                    add_axis(input_code::key_d);
                    add_axis(input_code::key_e);
                    add_axis(input_code::key_f);
                    add_axis(input_code::key_g);
                    add_axis(input_code::key_h);
                    add_axis(input_code::key_i);
                    add_axis(input_code::key_j);
                    add_axis(input_code::key_k);
                    add_axis(input_code::key_l);
                    add_axis(input_code::key_m);
                    add_axis(input_code::key_n);
                    add_axis(input_code::key_o);
                    add_axis(input_code::key_p);
                    add_axis(input_code::key_q);
                    add_axis(input_code::key_r);
                    add_axis(input_code::key_s);
                    add_axis(input_code::key_t);
                    add_axis(input_code::key_u);
                    add_axis(input_code::key_v);
                    add_axis(input_code::key_w);
                    add_axis(input_code::key_x);
                    add_axis(input_code::key_y);
                    add_axis(input_code::key_z);
                    add_axis(input_code::key_backspace);
                    add_axis(input_code::key_tab);
                    add_axis(input_code::key_clear);
                    add_axis(input_code::key_enter);
                    add_axis(input_code::key_pause);
                    add_axis(input_code::key_caps_lock);
                    add_axis(input_code::key_escape);
                    add_axis(input_code::key_space);
                    add_axis(input_code::key_page_up);
                    add_axis(input_code::key_page_down);
                    add_axis(input_code::key_end);
                    add_axis(input_code::key_home);
                    add_axis(input_code::key_left_arrow);
                    add_axis(input_code::key_up_arrow);
                    add_axis(input_code::key_right_arrow);
                    add_axis(input_code::key_down_arrow);
                    add_axis(input_code::key_print_screen);
                    add_axis(input_code::key_insert);
                    add_axis(input_code::key_delete);
                    add_axis(input_code::key_num_0);
                    add_axis(input_code::key_num_1);
                    add_axis(input_code::key_num_2);
                    add_axis(input_code::key_num_3);
                    add_axis(input_code::key_num_4);
                    add_axis(input_code::key_num_5);
                    add_axis(input_code::key_num_6);
                    add_axis(input_code::key_num_7);
                    add_axis(input_code::key_num_8);
                    add_axis(input_code::key_num_9);
                    add_axis(input_code::key_num_multiply);
                    add_axis(input_code::key_num_plus);
                    add_axis(input_code::key_num_enter);
                    add_axis(input_code::key_num_minus);
                    add_axis(input_code::key_num_decimal);
                    add_axis(input_code::key_num_divide);
                    add_axis(input_code::key_f1);
                    add_axis(input_code::key_f2);
                    add_axis(input_code::key_f3);
                    add_axis(input_code::key_f4);
                    add_axis(input_code::key_f5);
                    add_axis(input_code::key_f6);
                    add_axis(input_code::key_f7);
                    add_axis(input_code::key_f8);
                    add_axis(input_code::key_f9);
                    add_axis(input_code::key_f10);
                    add_axis(input_code::key_f11);
                    add_axis(input_code::key_f12);
                    add_axis(input_code::key_f13);
                    add_axis(input_code::key_f14);
                    add_axis(input_code::key_f15);
                    add_axis(input_code::key_f16);
                    add_axis(input_code::key_f17);
                    add_axis(input_code::key_f18);
                    add_axis(input_code::key_f19);
                    add_axis(input_code::key_f20);
                    add_axis(input_code::key_f21);
                    add_axis(input_code::key_f22);
                    add_axis(input_code::key_f23);
                    add_axis(input_code::key_f24);
                    add_axis(input_code::key_num_lock);
                    add_axis(input_code::key_scroll_lock);
                    add_axis(input_code::key_left_shift);
                    add_axis(input_code::key_right_shift);
                    add_axis(input_code::key_left_control);
                    add_axis(input_code::key_right_control);
                    add_axis(input_code::key_left_alt);
                    add_axis(input_code::key_right_alt);
                    add_axis(input_code::key_left_command);
                    add_axis(input_code::key_right_command);
                    add_axis(input_code::key_apps);
                    add_axis(input_code::key_plus);
                    add_axis(input_code::key_comma);
                    add_axis(input_code::key_minus);
                    add_axis(input_code::key_period);
                    add_axis(input_code::key_semicolon);
                    add_axis(input_code::key_slash);
                    add_axis(input_code::key_accent);
                    add_axis(input_code::key_left_bracket);
                    add_axis(input_code::key_backslash);
                    add_axis(input_code::key_right_bracket);
                    add_axis(input_code::key_quote);
                    add_axis(input_code::key_oem_8);
                    add_axis(input_code::key_oem_102);
                    break;
                case RIM_TYPEMOUSE:
                    add_axis(input_code::mouse_x);
                    add_axis(input_code::mouse_x_left);
                    add_axis(input_code::mouse_x_right);
                    add_axis(input_code::mouse_y);
                    add_axis(input_code::mouse_y_up);
                    add_axis(input_code::mouse_y_down);
                    add_axis(input_code::mouse_wheel);
                    add_axis(input_code::mouse_wheel_up);
                    add_axis(input_code::mouse_wheel_down);

                    // TODO horizontal wheel

                    add_axis(input_code::mouse_left);
                    add_axis(input_code::mouse_right);

                    if (info.mouse.dwNumberOfButtons > 2) {
                        add_axis(input_code::mouse_middle);
                    }

                    if (info.mouse.dwNumberOfButtons > 3) {
                        add_axis(input_code::mouse_fourth);
                    }

                    if (info.mouse.dwNumberOfButtons > 4) {
                        add_axis(input_code::mouse_fifth);
                    }
                    break;
                default:
                    set_usable(false);
                    break;
            }
        }

        raw_input_device::~raw_input_device() {
        }

        void raw_input_device::update(const RAWINPUT& packet) {
            RB_TRACE_ENTER();

            switch (packet.header.dwType) {
                case RIM_TYPEKEYBOARD:
                    RB_TRACE("in RIM_TYPEKEYBOARD");
                    update_keyboard(packet.data.keyboard);
                break;
                case RIM_TYPEMOUSE:
                    RB_TRACE("in RIM_TYPEMOUSE");
                    update_mouse(packet.data.mouse);
                break;
                case RIM_TYPEHID:
                    RB_TRACE("in RIM_TYPEHID");
                    m_ctx->log_debug("ri: Parsed WM_INPUT as raw HID report, ignoring");
                break;
                default:
                    RB_TRACE("in default");
                    m_ctx->log_debug("ri: Parsed WM_INPUT as unknown packet, ignoring");
                break;
            }
        }

        void raw_input_device::update_mouse_button(input_code code, unsigned short button_flags, unsigned short is_up_flag, unsigned short is_down_flag) {
            auto is_up = (button_flags & is_up_flag) != 0;
            auto is_down = (button_flags & is_down_flag) != 0;

            if (!is_up && !is_down) {
                return;
            }

            m_ctx->log_verbose("ri: mouse: button %1% new state = %2%", static_cast<int>(code), is_down);

            auto axis = get_axis(code);
            axis->set(is_down ? 1.0f : 0.0f);
        }

        void raw_input_device::update_mouse_axis(input_code code, float value) {
            if (fabs(value) < 0.00001f) {
                return;
            }

            m_ctx->log_verbose("ri: mouse: axis %1% new state = %2%", static_cast<int>(code), value);

            auto axis = get_axis(code);
            axis->add(value);
        }

        void raw_input_device::update_mouse(const RAWMOUSE& data) {
            auto flags = data.usFlags;
            auto button_flags = data.usButtonFlags;
            auto update_pos = false;
            float x = data.lLastX;
            float y = data.lLastY;
            float wheel = static_cast<short>(data.usButtonData);

            if ((flags & MOUSE_MOVE_ABSOLUTE) != 0) {
                // TODO cleanup this
                auto width = GetSystemMetrics(SM_CXSCREEN);
                auto height = GetSystemMetrics(SM_CYSCREEN);

                auto new_x = (x / 65535.0f) * width;
                auto new_y = (y / 65535.0f) * height;

                if (m_has_last_input) {
                    x = new_x - x;
                    y = new_y - y;
                    update_pos = true;
                }

                m_has_last_input = true;
            } else {
                update_pos = true;
            }

            if ((button_flags & RI_MOUSE_WHEEL) == 0) {
                wheel = 0;
            } else {
                wheel /= static_cast<float>(WHEEL_DELTA);
            }

            update_mouse_axis(input_code::mouse_wheel, wheel);
            if (update_pos) {
                update_mouse_axis(input_code::mouse_x, x);
                update_mouse_axis(input_code::mouse_y, y);
            }

            update_mouse_button(input_code::mouse_left, button_flags, RI_MOUSE_BUTTON_1_UP, RI_MOUSE_BUTTON_1_DOWN);
            update_mouse_button(input_code::mouse_right, button_flags, RI_MOUSE_BUTTON_2_UP, RI_MOUSE_BUTTON_2_DOWN);
            update_mouse_button(input_code::mouse_middle, button_flags, RI_MOUSE_BUTTON_3_UP, RI_MOUSE_BUTTON_3_DOWN);
            update_mouse_button(input_code::mouse_fourth, button_flags, RI_MOUSE_BUTTON_4_UP, RI_MOUSE_BUTTON_4_DOWN);
            update_mouse_button(input_code::mouse_fifth, button_flags, RI_MOUSE_BUTTON_5_UP, RI_MOUSE_BUTTON_5_DOWN);
        }

        UINT fixup_vkey_code(UINT scan_code, UINT vkey_code, bool is_e0) {
            // Before mapping to our codes, fix up the reported vkey
            // Ctrl/Alt should be done with MapVirtualKey but apparently that's broken woooo
            switch (vkey_code) {
                case 0xFF:
                    return 0;
                case VK_SHIFT:
                    return MapVirtualKey(scan_code, MAPVK_VSC_TO_VK_EX);
                case VK_CONTROL:
                    return is_e0 ? VK_RCONTROL : VK_LCONTROL;
                case VK_MENU:
                    return is_e0 ? VK_RMENU : VK_LMENU;
                case VK_RETURN:
                    return is_e0 ? VK_SEPARATOR : VK_RETURN;
                case VK_DELETE:
                    return is_e0 ? VK_DELETE : VK_DECIMAL;
                case VK_INSERT:
                    return is_e0 ? VK_INSERT : VK_NUMPAD0;
                case VK_END:
                    return is_e0 ? VK_END : VK_NUMPAD1;
                case VK_DOWN:
                    return is_e0 ? VK_DOWN : VK_NUMPAD2;
                case VK_NEXT:
                    return is_e0 ? VK_NEXT : VK_NUMPAD3;
                case VK_LEFT:
                    return is_e0 ? VK_LEFT : VK_NUMPAD4;
                case VK_CLEAR:
                    return is_e0 ? VK_CLEAR : VK_NUMPAD5;
                case VK_RIGHT:
                    return is_e0 ? VK_RIGHT : VK_NUMPAD6;
                case VK_HOME:
                    return is_e0 ? VK_HOME : VK_NUMPAD7;
                case VK_UP:
                    return is_e0 ? VK_UP : VK_NUMPAD8;
                case VK_PRIOR:
                    return is_e0 ? VK_PRIOR : VK_NUMPAD9;
                default:
                    return vkey_code;
            }
        }

        input_code map_to_input_code(UINT vkey_code) {
            switch (vkey_code) {
                default: return input_code::none;

                case '0': return input_code::key_0;
                case '1': return input_code::key_1;
                case '2': return input_code::key_2;
                case '3': return input_code::key_3;
                case '4': return input_code::key_4;
                case '5': return input_code::key_5;
                case '6': return input_code::key_6;
                case '7': return input_code::key_7;
                case '8': return input_code::key_8;
                case '9': return input_code::key_9;
                case 'A': return input_code::key_a;
                case 'B': return input_code::key_b;
                case 'C': return input_code::key_c;
                case 'D': return input_code::key_d;
                case 'E': return input_code::key_e;
                case 'F': return input_code::key_f;
                case 'G': return input_code::key_g;
                case 'H': return input_code::key_h;
                case 'I': return input_code::key_i;
                case 'J': return input_code::key_j;
                case 'K': return input_code::key_k;
                case 'L': return input_code::key_l;
                case 'M': return input_code::key_m;
                case 'N': return input_code::key_n;
                case 'O': return input_code::key_o;
                case 'P': return input_code::key_p;
                case 'Q': return input_code::key_q;
                case 'R': return input_code::key_r;
                case 'S': return input_code::key_s;
                case 'T': return input_code::key_t;
                case 'U': return input_code::key_u;
                case 'V': return input_code::key_v;
                case 'W': return input_code::key_w;
                case 'X': return input_code::key_x;
                case 'Y': return input_code::key_y;
                case 'Z': return input_code::key_z;

                case VK_BACK: return input_code::key_backspace;
                case VK_TAB: return input_code::key_tab;
                case VK_CLEAR: return input_code::key_clear;
                case VK_RETURN: return input_code::key_enter;
                case VK_PAUSE: return input_code::key_pause;
                case VK_CAPITAL: return input_code::key_caps_lock;
                case VK_ESCAPE: return input_code::key_escape;
                case VK_SPACE: return input_code::key_space;
                case VK_PRIOR: return input_code::key_page_up;
                case VK_NEXT: return input_code::key_page_down;
                case VK_END: return input_code::key_end;
                case VK_HOME: return input_code::key_home;
                case VK_LEFT: return input_code::key_left_arrow;
                case VK_UP: return input_code::key_up_arrow;
                case VK_RIGHT: return input_code::key_right_arrow;
                case VK_DOWN: return input_code::key_down_arrow;
                case VK_SNAPSHOT: return input_code::key_print_screen;
                case VK_INSERT: return input_code::key_insert;
                case VK_DELETE: return input_code::key_delete;
                case VK_NUMPAD0: return input_code::key_num_0;
                case VK_NUMPAD1: return input_code::key_num_1;
                case VK_NUMPAD2: return input_code::key_num_2;
                case VK_NUMPAD3: return input_code::key_num_3;
                case VK_NUMPAD4: return input_code::key_num_4;
                case VK_NUMPAD5: return input_code::key_num_5;
                case VK_NUMPAD6: return input_code::key_num_6;
                case VK_NUMPAD7: return input_code::key_num_7;
                case VK_NUMPAD8: return input_code::key_num_8;
                case VK_NUMPAD9: return input_code::key_num_9;
                case VK_MULTIPLY: return input_code::key_num_multiply;
                case VK_ADD: return input_code::key_num_plus;
                case VK_SEPARATOR: return input_code::key_num_enter;
                case VK_SUBTRACT: return input_code::key_num_minus;
                case VK_DECIMAL: return input_code::key_num_decimal;
                case VK_DIVIDE: return input_code::key_num_divide;
                case VK_F1: return input_code::key_f1;
                case VK_F2: return input_code::key_f2;
                case VK_F3: return input_code::key_f3;
                case VK_F4: return input_code::key_f4;
                case VK_F5: return input_code::key_f5;
                case VK_F6: return input_code::key_f6;
                case VK_F7: return input_code::key_f7;
                case VK_F8: return input_code::key_f8;
                case VK_F9: return input_code::key_f9;
                case VK_F10: return input_code::key_f10;
                case VK_F11: return input_code::key_f11;
                case VK_F12: return input_code::key_f12;
                case VK_F13: return input_code::key_f13;
                case VK_F14: return input_code::key_f14;
                case VK_F15: return input_code::key_f15;
                case VK_F16: return input_code::key_f16;
                case VK_F17: return input_code::key_f17;
                case VK_F18: return input_code::key_f18;
                case VK_F19: return input_code::key_f19;
                case VK_F20: return input_code::key_f20;
                case VK_F21: return input_code::key_f21;
                case VK_F22: return input_code::key_f22;
                case VK_F23: return input_code::key_f23;
                case VK_F24: return input_code::key_f24;
                case VK_NUMLOCK: return input_code::key_num_lock;
                case VK_SCROLL: return input_code::key_scroll_lock;
                case VK_LSHIFT: return input_code::key_left_shift;
                case VK_RSHIFT: return input_code::key_right_shift;
                case VK_LCONTROL: return input_code::key_left_control;
                case VK_RCONTROL: return input_code::key_right_control;
                case VK_LMENU: return input_code::key_left_alt;
                case VK_RMENU: return input_code::key_right_alt;
                case VK_LWIN: return input_code::key_left_command;
                case VK_RWIN: return input_code::key_right_command;
                case VK_APPS: return input_code::key_apps;
                case VK_OEM_PLUS: return input_code::key_plus;
                case VK_OEM_COMMA: return input_code::key_comma;
                case VK_OEM_MINUS: return input_code::key_minus;
                case VK_OEM_PERIOD: return input_code::key_period;
                case VK_OEM_1: return input_code::key_semicolon;
                case VK_OEM_2: return input_code::key_slash;
                case VK_OEM_3: return input_code::key_accent;
                case VK_OEM_4: return input_code::key_left_bracket;
                case VK_OEM_5: return input_code::key_backslash;
                case VK_OEM_6: return input_code::key_right_bracket;
                case VK_OEM_7: return input_code::key_quote;
                case VK_OEM_8: return input_code::key_oem_8;
                case VK_OEM_102: return input_code::key_oem_102;
            }
        }

        void raw_input_device::update_keyboard(const RAWKEYBOARD& data) {
            UINT scan_code = data.MakeCode;
            UINT vkey_code = data.VKey;
            auto flags = data.Flags;

            auto is_e0 = (flags & RI_KEY_E0) != 0;
            auto is_up = (flags & RI_KEY_BREAK) != 0;

            // Windows pls
            vkey_code = fixup_vkey_code(scan_code, vkey_code, is_e0);
            if (vkey_code == 0) {
                return;
            }

            auto code = map_to_input_code(vkey_code);
            if (code == input_code::none) {
                return;
            }

            auto axis = get_axis(code);
            axis->set(is_up ? 0.0f : 1.0f);
        }

        void raw_input_device::commit() {
            derive_mouse_pre_commit(*this);
            device::commit();
            derive_mouse_post_commit(*this);
        }
    }
}
