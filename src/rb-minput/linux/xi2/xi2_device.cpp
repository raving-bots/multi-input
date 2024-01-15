// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "linux/xi2/xi2_device.hpp"
#include "linux/xi2/x11.hpp"
#include "linux/fs.hpp"
#include "linux/udev_info.hpp"
#include "axis_utils.hpp"
#include "context.hpp"

#include <boost/algorithm/string/predicate.hpp>
#include <X11/Xatom.h>

namespace multi_input {
    namespace lnx {
        namespace detail {
            struct atoms {
                explicit atoms(Display *display) :
                    m_display(display), m_rel_x(intern("Rel X")), m_rel_y(intern("Rel Y")),
                    m_rel_horiz_wheel(intern("Rel Horiz Wheel")), m_rel_vert_wheel(intern("Rel Vert Wheel"))
                {
                }

                Atom intern(const char* label) {
                    return XInternAtom(m_display, label, True);
                }

                Display *m_display;
                Atom m_rel_x;
                Atom m_rel_y;
                Atom m_rel_horiz_wheel;
                Atom m_rel_vert_wheel;
            };

            std::string get_device_node(Display* display, int device_id) {
                auto device_node = XInternAtom(display, "Device Node", True);
                if (device_node == None) {
                    return "<unknown>";
                }

                Atom type{};
                int format{};
                unsigned long count{};
                unsigned char* data{};
                unsigned long remaining{};

                auto status = XIGetProperty(
                    display, device_id, device_node,
                    0, 1024, False,
                    XA_STRING, &type, &format,
                    &count, &remaining, &data
                );

                if (status != Success || type != XA_STRING || format != 8 || remaining > 0) {
                    return "<XIGetProperty failed>";
                }

                auto it = const_cast<const char*>(reinterpret_cast<char*>(data));
                std::string result{it};
                XFree(data);

                return result;
            }

            int try_parse_id(std::string value) {
                if (value.empty()) {
                    return 0;
                }

                try {
                    return std::stoi(value, nullptr, 16);
                } catch (const std::exception&) {
                    return 0;
                }
            }

            void update_meta(context *ctx, device_id id, Display *display, XIDeviceInfo& info, device_meta& meta) {
                meta.set_name(info.name);

                auto device_node = get_device_node(display, info.deviceid);
                auto symbolic_name = fs::filename(device_node);

                auto sysfs_path = fs::join(fs::sysfs_path(), symbolic_name);
                sysfs_path = fs::canonical(sysfs_path);

                // TODO probably need to loop through parents sigh for proper sysfs use
                auto parent_name = fs::filename(fs::parent(sysfs_path));
                if (!boost::starts_with(parent_name, "input")) {
                    ctx->log_warning("xi2: update_meta: %1% parent not inputNN: %2%", id, parent_name);
                    return;
                }

                udev_info props{"+input:" + parent_name};
                meta.set_location(props.get("ID_PATH"));
                meta.set_internal_id(device_node);
                meta.set_ids(
                    try_parse_id(props.get("ID_VENDOR_ID")),
                    try_parse_id(props.get("ID_MODEL_ID")),
                    try_parse_id(props.get("ID_REVISION"))
                );
                meta.set_serial(props.get("ID_SERIAL"));
            }
        }

        xi2_device::xi2_device(context *ctx, device_id id, Display* display, XIDeviceInfo& info) :
            device(ctx, id),
            m_x11_id(info.deviceid), m_display(display),
            m_axis_rel_x(-1), m_axis_rel_y(-1), m_axis_rel_horiz_wheel(-1), m_axis_rel_vert_wheel(-1)
        {
            detail::atoms atoms{display};

            detail::update_meta(m_ctx, id, display, info, get_meta());
            set_usable(info.enabled);

            for (int idx = 0; idx < info.num_classes; ++idx) {
                auto&& any_class = *info.classes[idx];

                // TODO maybe scroll class to better distinguish wheels?
                switch (any_class.type) {
                    case XIValuatorClass:
                        add_valuator_class(reinterpret_cast<XIValuatorClassInfo&>(any_class), atoms);
                        break;
                    case XIButtonClass:
                        add_button_class(reinterpret_cast<XIButtonClassInfo&>(any_class));
                        break;
                    case XIKeyClass:
                        add_key_class(reinterpret_cast<XIKeyClassInfo&>(any_class));
                        break;
                }
            }
        }

        xi2_device::~xi2_device() {
        }

        void xi2_device::add_valuator_class(XIValuatorClassInfo& valuator, const detail::atoms& atoms) {
            RB_TRACE_ENTER();

            if (valuator.mode == XIModeAbsolute) {
                // TODO API support for absolute axes
                RB_TRACE("skipping absolute valuator");
                return;
            }

            // XXX verify that comparing atoms is really the way to go
            if (valuator.label == atoms.m_rel_x) {
                m_axis_rel_x = valuator.number;

                add_axis(input_code::mouse_x_left);
                add_axis(input_code::mouse_x_right);
                add_axis(input_code::mouse_x);
            } else if (valuator.label == atoms.m_rel_y) {
                m_axis_rel_y = valuator.number;

                add_axis(input_code::mouse_y_up);
                add_axis(input_code::mouse_y_down);
                add_axis(input_code::mouse_y);
            } else if (valuator.label == atoms.m_rel_horiz_wheel) {
                // TODO API support for horizontal wheel
                m_axis_rel_horiz_wheel = valuator.number;
            } else if (valuator.label == atoms.m_rel_vert_wheel) {
                m_axis_rel_vert_wheel = valuator.number;

                add_axis(input_code::mouse_wheel_up);
                add_axis(input_code::mouse_wheel_down);
                add_axis(input_code::mouse_wheel);
            }
        }

        void xi2_device::add_button_class(XIButtonClassInfo& buttons) {
            if (buttons.num_buttons >= 3) {
                // has basic buttons
                add_axis(input_code::mouse_left);
                add_axis(input_code::mouse_middle);
                add_axis(input_code::mouse_right);
            }

            // buttons 4, 5, 6, 7 are wheels

            if (buttons.num_buttons >= 8) {
                add_axis(input_code::mouse_fourth);
            }

            if (buttons.num_buttons >= 9) {
                add_axis(input_code::mouse_fifth);
            }

            if (buttons.num_buttons >= 10) {
                add_axis(input_code::mouse_sixth);
            }

            if (buttons.num_buttons >= 11) {
                add_axis(input_code::mouse_seventh);
            }
        }

        input_code xi2_device::map_key_code(int key_code) {
            auto key_sym = XkbKeycodeToKeysym(m_display, key_code, 0, 0);

            switch (key_sym) {
                default:
                    return input_code::none;

                case XK_BackSpace:       return input_code::key_backspace;
                case XK_Tab:             return input_code::key_tab;
                case XK_Linefeed:        return input_code::key_enter;
                case XK_Clear:           return input_code::key_clear;
                case XK_Return:          return input_code::key_enter;
                case XK_Pause:           return input_code::key_pause;
                case XK_Scroll_Lock:     return input_code::key_scroll_lock;
                case XK_Escape:          return input_code::key_escape;
                case XK_Delete:          return input_code::key_delete;
                case XK_Home:            return input_code::key_home;
                case XK_Left:            return input_code::key_left_arrow;
                case XK_Up:              return input_code::key_up_arrow;
                case XK_Right:           return input_code::key_right_arrow;
                case XK_Down:            return input_code::key_down_arrow;
                case XK_Page_Up:         return input_code::key_page_up;
                case XK_Page_Down:       return input_code::key_page_down;
                case XK_End:             return input_code::key_end;
                case XK_Begin:           return input_code::key_home;
                case XK_Print:           return input_code::key_print_screen;
                case XK_Insert:          return input_code::key_insert;
                case XK_Menu:            return input_code::key_apps;
                case XK_Num_Lock:        return input_code::key_num_lock;
                case XK_KP_Space:        return input_code::key_space;
                case XK_KP_Tab:          return input_code::key_tab;
                case XK_KP_Enter:        return input_code::key_num_enter;
                case XK_KP_Home:         return input_code::key_num_7;
                case XK_KP_Left:         return input_code::key_num_4;
                case XK_KP_Up:           return input_code::key_num_8;
                case XK_KP_Right:        return input_code::key_num_6;
                case XK_KP_Down:         return input_code::key_num_2;
                case XK_KP_Page_Up:      return input_code::key_num_9;
                case XK_KP_Page_Down:    return input_code::key_num_3;
                case XK_KP_End:          return input_code::key_num_1;
                case XK_KP_Begin:        return input_code::key_num_5;
                case XK_KP_Insert:       return input_code::key_num_0;
                case XK_KP_Delete:       return input_code::key_num_decimal;
                case XK_KP_Multiply:     return input_code::key_num_multiply;
                case XK_KP_Add:          return input_code::key_num_plus;
                case XK_KP_Subtract:     return input_code::key_num_minus;
                case XK_KP_Separator:    return input_code::key_num_decimal;
                case XK_KP_Decimal:      return input_code::key_num_decimal;
                case XK_KP_Divide:       return input_code::key_num_divide;
                case XK_KP_0:            return input_code::key_num_0;
                case XK_KP_1:            return input_code::key_num_1;
                case XK_KP_2:            return input_code::key_num_2;
                case XK_KP_3:            return input_code::key_num_3;
                case XK_KP_4:            return input_code::key_num_4;
                case XK_KP_5:            return input_code::key_num_5;
                case XK_KP_6:            return input_code::key_num_6;
                case XK_KP_7:            return input_code::key_num_7;
                case XK_KP_8:            return input_code::key_num_8;
                case XK_KP_9:            return input_code::key_num_9;
                case XK_F1:              return input_code::key_f1;
                case XK_F2:              return input_code::key_f2;
                case XK_F3:              return input_code::key_f3;
                case XK_F4:              return input_code::key_f4;
                case XK_F5:              return input_code::key_f5;
                case XK_F6:              return input_code::key_f6;
                case XK_F7:              return input_code::key_f7;
                case XK_F8:              return input_code::key_f8;
                case XK_F9:              return input_code::key_f9;
                case XK_F10:             return input_code::key_f10;
                case XK_F11:             return input_code::key_f11;
                case XK_F12:             return input_code::key_f12;
                case XK_F13:             return input_code::key_f13;
                case XK_F14:             return input_code::key_f14;
                case XK_F15:             return input_code::key_f15;
                case XK_F16:             return input_code::key_f16;
                case XK_F17:             return input_code::key_f17;
                case XK_F18:             return input_code::key_f18;
                case XK_F19:             return input_code::key_f19;
                case XK_F20:             return input_code::key_f20;
                case XK_F21:             return input_code::key_f21;
                case XK_F22:             return input_code::key_f22;
                case XK_F23:             return input_code::key_f23;
                case XK_F24:             return input_code::key_f24;
                case XK_Shift_L:         return input_code::key_left_shift;
                case XK_Shift_R:         return input_code::key_right_shift;
                case XK_Control_L:       return input_code::key_left_control;
                case XK_Control_R:       return input_code::key_right_control;
                case XK_Caps_Lock:       return input_code::key_caps_lock;
                case XK_Alt_L:           return input_code::key_left_alt;
                case XK_Alt_R:           return input_code::key_right_alt;
                case XK_Super_L:         return input_code::key_left_command;
                case XK_Super_R:         return input_code::key_right_command;
                case XK_ISO_Level3_Shift:return input_code::key_right_alt;
                case XK_space:           return input_code::key_space;
                case XK_apostrophe:      return input_code::key_quote;
                case XK_comma:           return input_code::key_comma;
                case XK_minus:           return input_code::key_minus;
                case XK_period:          return input_code::key_period;
                case XK_slash:           return input_code::key_slash;
                case XK_semicolon:       return input_code::key_semicolon;
                case XK_equal:           return input_code::key_plus;
                case XK_bracketleft:     return input_code::key_left_bracket;
                case XK_backslash:       return input_code::key_backslash;
                case XK_bracketright:    return input_code::key_right_bracket;
                case XK_grave:           return input_code::key_accent;
                case XK_0:               return input_code::key_0;
                case XK_1:               return input_code::key_1;
                case XK_2:               return input_code::key_2;
                case XK_3:               return input_code::key_3;
                case XK_4:               return input_code::key_4;
                case XK_5:               return input_code::key_5;
                case XK_6:               return input_code::key_6;
                case XK_7:               return input_code::key_7;
                case XK_8:               return input_code::key_8;
                case XK_9:               return input_code::key_9;
                case XK_a:               return input_code::key_a;
                case XK_b:               return input_code::key_b;
                case XK_c:               return input_code::key_c;
                case XK_d:               return input_code::key_d;
                case XK_e:               return input_code::key_e;
                case XK_f:               return input_code::key_f;
                case XK_g:               return input_code::key_g;
                case XK_h:               return input_code::key_h;
                case XK_i:               return input_code::key_i;
                case XK_j:               return input_code::key_j;
                case XK_k:               return input_code::key_k;
                case XK_l:               return input_code::key_l;
                case XK_m:               return input_code::key_m;
                case XK_n:               return input_code::key_n;
                case XK_o:               return input_code::key_o;
                case XK_p:               return input_code::key_p;
                case XK_q:               return input_code::key_q;
                case XK_r:               return input_code::key_r;
                case XK_s:               return input_code::key_s;
                case XK_t:               return input_code::key_t;
                case XK_u:               return input_code::key_u;
                case XK_v:               return input_code::key_v;
                case XK_w:               return input_code::key_w;
                case XK_x:               return input_code::key_x;
                case XK_y:               return input_code::key_y;
                case XK_z:               return input_code::key_z;
            }
        }

        void xi2_device::add_key_class(XIKeyClassInfo& keys) {
            for (int idx = 0; idx < keys.num_keycodes; ++idx) {
                auto key_code = keys.keycodes[idx];
                auto axis = map_key_code(key_code);
                add_axis(axis);
            }
        }

        void xi2_device::update(XIRawEvent& event) {
            switch (event.evtype) {
                case XI_RawKeyPress:
                case XI_RawKeyRelease:
                    update_key(event);
                    break;
                case XI_RawButtonPress:
                case XI_RawButtonRelease:
                    update_button(event);
                    break;
                case XI_RawMotion:
                    update_motion(event);
                    break;
            }
        }

        void xi2_device::update_key(XIRawEvent& event) {
            auto code = map_key_code(event.detail);
            auto axis = get_axis(code);

            if (code == input_code::none) {
                return;
            }

            if (axis == nullptr) {
                m_ctx->log_warning(
                    u8"XI2: possible bug: got key code %1% (mapped %2%) but it wasn't added during discovery",
                    event.detail, static_cast<int>(code)
                );
                axis = add_axis(code);
            }

            m_ctx->log_verbose(
                u8"XI2: %1%: axis %2% device %3%",
                event.evtype == XI_RawKeyPress ? u8"key press" : u8"key release",
                static_cast<int>(code), m_id
            );
            axis->set(event.evtype == XI_RawKeyPress ? 1.0f : 0.0f);
        }

        void xi2_device::update_motion(XIRawEvent& event) {
            auto valuator = event.raw_values;

            for (int idx = 0; idx < event.valuators.mask_len * 8; ++idx) {
                if (!XIMaskIsSet(event.valuators.mask, idx)) {
                    continue;
                }

                auto value = *valuator++;
                virtual_axis *axis = nullptr;

                if (idx == m_axis_rel_x) {
                    axis = get_axis(input_code::mouse_x);
                } else if (idx == m_axis_rel_y) {
                    axis = get_axis(input_code::mouse_y);
                } else if (idx == m_axis_rel_vert_wheel) {
                    axis = get_axis(input_code::mouse_wheel);
                }
                // TODO horizontal wheel

                if (axis == nullptr) {
                    continue;
                }

                axis->add(static_cast<float>(value));
            }
        }

        input_code xi2_device::map_button_code(int button) {
            switch (button) {
                default:
                    return input_code::none;

                case 1:  return input_code::mouse_left;
                case 2:  return input_code::mouse_middle;
                case 3:  return input_code::mouse_right;
                case 8:  return input_code::mouse_fourth;
                case 9:  return input_code::mouse_fifth;
                case 10: return input_code::mouse_sixth;
                case 11: return input_code::mouse_seventh;
            }
        }

        void xi2_device::update_button(XIRawEvent& event) {
            auto code = map_button_code(event.detail);
            auto axis = get_axis(code);

            if (code == input_code::none) {
                return;
            }

            if (axis == nullptr) {
                m_ctx->log_warning(
                    u8"XI2: possible bug: got button code %1% (mapped %2%) but it wasn't added during discovery",
                    event.detail, static_cast<int>(code)
                );
                axis = add_axis(code);
            }

            m_ctx->log_verbose(
                u8"XI2: %1%: axis %2% device %3%",
                event.evtype == XI_RawButtonPress ? u8"button press" : u8"button release",
                static_cast<int>(code), m_id
            );
            axis->set(event.evtype == XI_RawButtonPress ? 1.0f : 0.0f);
        }

        void xi2_device::commit() {
            derive_mouse_pre_commit(*this);
            device::commit();
            derive_mouse_post_commit(*this);
        }
    }
}
