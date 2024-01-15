// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <utility>

// TODO fix generation
namespace multi_input {
    enum class input_code : int {
        none = 0,
        key_0 = 100000,
        key_1 = 100001,
        key_2 = 100002,
        key_3 = 100003,
        key_4 = 100004,
        key_5 = 100005,
        key_6 = 100006,
        key_7 = 100007,
        key_8 = 100008,
        key_9 = 100009,
        key_a = 100010,
        key_b = 100011,
        key_c = 100012,
        key_d = 100013,
        key_e = 100014,
        key_f = 100015,
        key_g = 100016,
        key_h = 100017,
        key_i = 100018,
        key_j = 100019,
        key_k = 100020,
        key_l = 100021,
        key_m = 100022,
        key_n = 100023,
        key_o = 100024,
        key_p = 100025,
        key_q = 100026,
        key_r = 100027,
        key_s = 100028,
        key_t = 100029,
        key_u = 100030,
        key_v = 100031,
        key_w = 100032,
        key_x = 100033,
        key_y = 100034,
        key_z = 100035,
        key_semicolon = 100036,
        key_slash = 100037,
        key_accent = 100038,
        key_left_bracket = 100039,
        key_backslash = 100040,
        key_right_bracket = 100041,
        key_quote = 100042,
        key_oem_8 = 100043,
        key_oem_102 = 100044,
        key_backspace = 100045,
        key_tab = 100046,
        key_clear = 100047,
        key_enter = 100048,
        key_escape = 100049,
        key_space = 100050,
        key_plus = 100051,
        key_comma = 100052,
        key_minus = 100053,
        key_period = 100054,
        key_num_0 = 100055,
        key_num_1 = 100056,
        key_num_2 = 100057,
        key_num_3 = 100058,
        key_num_4 = 100059,
        key_num_5 = 100060,
        key_num_6 = 100061,
        key_num_7 = 100062,
        key_num_8 = 100063,
        key_num_9 = 100064,
        key_num_decimal = 100065,
        key_num_divide = 100066,
        key_num_multiply = 100067,
        key_num_minus = 100068,
        key_num_plus = 100069,
        key_num_enter = 100070,
        key_up_arrow = 100071,
        key_down_arrow = 100072,
        key_right_arrow = 100073,
        key_left_arrow = 100074,
        key_insert = 100075,
        key_delete = 100076,
        key_home = 100077,
        key_end = 100078,
        key_page_up = 100079,
        key_page_down = 100080,
        key_f1 = 100081,
        key_f2 = 100082,
        key_f3 = 100083,
        key_f4 = 100084,
        key_f5 = 100085,
        key_f6 = 100086,
        key_f7 = 100087,
        key_f8 = 100088,
        key_f9 = 100089,
        key_f10 = 100090,
        key_f11 = 100091,
        key_f12 = 100092,
        key_f13 = 100093,
        key_f14 = 100094,
        key_f15 = 100095,
        key_f16 = 100096,
        key_f17 = 100097,
        key_f18 = 100098,
        key_f19 = 100099,
        key_f20 = 100100,
        key_f21 = 100101,
        key_f22 = 100102,
        key_f23 = 100103,
        key_f24 = 100104,
        key_right_shift = 100105,
        key_left_shift = 100106,
        key_right_alt = 100107,
        key_left_alt = 100108,
        key_right_control = 100109,
        key_left_control = 100110,
        key_right_command = 100111,
        key_left_command = 100112,
        key_apps = 100113,
        key_print_screen = 100114,
        key_pause = 100115,
        key_caps_lock = 100116,
        key_num_lock = 100117,
        key_scroll_lock = 100118,
        mouse_left = 200000,
        mouse_right = 200001,
        mouse_middle = 200002,
        mouse_fourth = 200003,
        mouse_fifth = 200004,
        mouse_sixth = 200005,
        mouse_seventh = 200006,
        mouse_x_left = 200007,
        mouse_x_right = 200008,
        mouse_y_up = 200009,
        mouse_y_down = 200010,
        mouse_wheel_up = 200011,
        mouse_wheel_down = 200012,
        mouse_x = 200013,
        mouse_y = 200014,
        mouse_wheel = 200015,
        pad_left_stick_up = 300000,
        pad_left_stick_down = 300001,
        pad_left_stick_left = 300002,
        pad_left_stick_right = 300003,
        pad_left_stick = 300004,
        pad_left_stick_x = 300005,
        pad_left_stick_y = 300006,
        pad_right_stick_up = 300007,
        pad_right_stick_down = 300008,
        pad_right_stick_left = 300009,
        pad_right_stick_right = 300010,
        pad_right_stick = 300011,
        pad_right_stick_x = 300012,
        pad_right_stick_y = 300013,
        pad_dpad_up = 300014,
        pad_dpad_down = 300015,
        pad_dpad_left = 300016,
        pad_dpad_right = 300017,
        pad_dpad_x = 300018,
        pad_dpad_y = 300019,
        pad_a = 300020,
        pad_b = 300021,
        pad_x = 300022,
        pad_y = 300023,
        pad_left_trigger = 300024,
        pad_right_trigger = 300025,
        pad_left_bumper = 300026,
        pad_right_bumper = 300027,
        pad_back = 300028,
        pad_start = 300029,
    };

    inline api_string to_string(input_code value) {
        switch (value) {
            case input_code::none: return "none";
            case input_code::key_0: return "key_0";
            case input_code::key_1: return "key_1";
            case input_code::key_2: return "key_2";
            case input_code::key_3: return "key_3";
            case input_code::key_4: return "key_4";
            case input_code::key_5: return "key_5";
            case input_code::key_6: return "key_6";
            case input_code::key_7: return "key_7";
            case input_code::key_8: return "key_8";
            case input_code::key_9: return "key_9";
            case input_code::key_a: return "key_a";
            case input_code::key_b: return "key_b";
            case input_code::key_c: return "key_c";
            case input_code::key_d: return "key_d";
            case input_code::key_e: return "key_e";
            case input_code::key_f: return "key_f";
            case input_code::key_g: return "key_g";
            case input_code::key_h: return "key_h";
            case input_code::key_i: return "key_i";
            case input_code::key_j: return "key_j";
            case input_code::key_k: return "key_k";
            case input_code::key_l: return "key_l";
            case input_code::key_m: return "key_m";
            case input_code::key_n: return "key_n";
            case input_code::key_o: return "key_o";
            case input_code::key_p: return "key_p";
            case input_code::key_q: return "key_q";
            case input_code::key_r: return "key_r";
            case input_code::key_s: return "key_s";
            case input_code::key_t: return "key_t";
            case input_code::key_u: return "key_u";
            case input_code::key_v: return "key_v";
            case input_code::key_w: return "key_w";
            case input_code::key_x: return "key_x";
            case input_code::key_y: return "key_y";
            case input_code::key_z: return "key_z";
            case input_code::key_semicolon: return "key_semicolon";
            case input_code::key_slash: return "key_slash";
            case input_code::key_accent: return "key_accent";
            case input_code::key_left_bracket: return "key_left_bracket";
            case input_code::key_backslash: return "key_backslash";
            case input_code::key_right_bracket: return "key_right_bracket";
            case input_code::key_quote: return "key_quote";
            case input_code::key_oem_8: return "key_oem_8";
            case input_code::key_oem_102: return "key_oem_102";
            case input_code::key_backspace: return "key_backspace";
            case input_code::key_tab: return "key_tab";
            case input_code::key_clear: return "key_clear";
            case input_code::key_enter: return "key_enter";
            case input_code::key_escape: return "key_escape";
            case input_code::key_space: return "key_space";
            case input_code::key_plus: return "key_plus";
            case input_code::key_comma: return "key_comma";
            case input_code::key_minus: return "key_minus";
            case input_code::key_period: return "key_period";
            case input_code::key_num_0: return "key_num_0";
            case input_code::key_num_1: return "key_num_1";
            case input_code::key_num_2: return "key_num_2";
            case input_code::key_num_3: return "key_num_3";
            case input_code::key_num_4: return "key_num_4";
            case input_code::key_num_5: return "key_num_5";
            case input_code::key_num_6: return "key_num_6";
            case input_code::key_num_7: return "key_num_7";
            case input_code::key_num_8: return "key_num_8";
            case input_code::key_num_9: return "key_num_9";
            case input_code::key_num_decimal: return "key_num_decimal";
            case input_code::key_num_divide: return "key_num_divide";
            case input_code::key_num_multiply: return "key_num_multiply";
            case input_code::key_num_minus: return "key_num_minus";
            case input_code::key_num_plus: return "key_num_plus";
            case input_code::key_num_enter: return "key_num_enter";
            case input_code::key_up_arrow: return "key_up_arrow";
            case input_code::key_down_arrow: return "key_down_arrow";
            case input_code::key_right_arrow: return "key_right_arrow";
            case input_code::key_left_arrow: return "key_left_arrow";
            case input_code::key_insert: return "key_insert";
            case input_code::key_delete: return "key_delete";
            case input_code::key_home: return "key_home";
            case input_code::key_end: return "key_end";
            case input_code::key_page_up: return "key_page_up";
            case input_code::key_page_down: return "key_page_down";
            case input_code::key_f1: return "key_f1";
            case input_code::key_f2: return "key_f2";
            case input_code::key_f3: return "key_f3";
            case input_code::key_f4: return "key_f4";
            case input_code::key_f5: return "key_f5";
            case input_code::key_f6: return "key_f6";
            case input_code::key_f7: return "key_f7";
            case input_code::key_f8: return "key_f8";
            case input_code::key_f9: return "key_f9";
            case input_code::key_f10: return "key_f10";
            case input_code::key_f11: return "key_f11";
            case input_code::key_f12: return "key_f12";
            case input_code::key_f13: return "key_f13";
            case input_code::key_f14: return "key_f14";
            case input_code::key_f15: return "key_f15";
            case input_code::key_f16: return "key_f16";
            case input_code::key_f17: return "key_f17";
            case input_code::key_f18: return "key_f18";
            case input_code::key_f19: return "key_f19";
            case input_code::key_f20: return "key_f20";
            case input_code::key_f21: return "key_f21";
            case input_code::key_f22: return "key_f22";
            case input_code::key_f23: return "key_f23";
            case input_code::key_f24: return "key_f24";
            case input_code::key_right_shift: return "key_right_shift";
            case input_code::key_left_shift: return "key_left_shift";
            case input_code::key_right_alt: return "key_right_alt";
            case input_code::key_left_alt: return "key_left_alt";
            case input_code::key_right_control: return "key_right_control";
            case input_code::key_left_control: return "key_left_control";
            case input_code::key_right_command: return "key_right_command";
            case input_code::key_left_command: return "key_left_command";
            case input_code::key_apps: return "key_apps";
            case input_code::key_print_screen: return "key_print_screen";
            case input_code::key_pause: return "key_pause";
            case input_code::key_caps_lock: return "key_caps_lock";
            case input_code::key_num_lock: return "key_num_lock";
            case input_code::key_scroll_lock: return "key_scroll_lock";
            case input_code::mouse_left: return "mouse_left";
            case input_code::mouse_right: return "mouse_right";
            case input_code::mouse_middle: return "mouse_middle";
            case input_code::mouse_fourth: return "mouse_fourth";
            case input_code::mouse_fifth: return "mouse_fifth";
            case input_code::mouse_sixth: return "mouse_sixth";
            case input_code::mouse_seventh: return "mouse_seventh";
            case input_code::mouse_x_left: return "mouse_x_left";
            case input_code::mouse_x_right: return "mouse_x_right";
            case input_code::mouse_y_up: return "mouse_y_up";
            case input_code::mouse_y_down: return "mouse_y_down";
            case input_code::mouse_wheel_up: return "mouse_wheel_up";
            case input_code::mouse_wheel_down: return "mouse_wheel_down";
            case input_code::mouse_x: return "mouse_x";
            case input_code::mouse_y: return "mouse_y";
            case input_code::mouse_wheel: return "mouse_wheel";
            case input_code::pad_left_stick_up: return "pad_left_stick_up";
            case input_code::pad_left_stick_down: return "pad_left_stick_down";
            case input_code::pad_left_stick_left: return "pad_left_stick_left";
            case input_code::pad_left_stick_right: return "pad_left_stick_right";
            case input_code::pad_left_stick: return "pad_left_stick";
            case input_code::pad_left_stick_x: return "pad_left_stick_x";
            case input_code::pad_left_stick_y: return "pad_left_stick_y";
            case input_code::pad_right_stick_up: return "pad_right_stick_up";
            case input_code::pad_right_stick_down: return "pad_right_stick_down";
            case input_code::pad_right_stick_left: return "pad_right_stick_left";
            case input_code::pad_right_stick_right: return "pad_right_stick_right";
            case input_code::pad_right_stick: return "pad_right_stick";
            case input_code::pad_right_stick_x: return "pad_right_stick_x";
            case input_code::pad_right_stick_y: return "pad_right_stick_y";
            case input_code::pad_dpad_up: return "pad_dpad_up";
            case input_code::pad_dpad_down: return "pad_dpad_down";
            case input_code::pad_dpad_left: return "pad_dpad_left";
            case input_code::pad_dpad_right: return "pad_dpad_right";
            case input_code::pad_dpad_x: return "pad_dpad_x";
            case input_code::pad_dpad_y: return "pad_dpad_y";
            case input_code::pad_a: return "pad_a";
            case input_code::pad_b: return "pad_b";
            case input_code::pad_x: return "pad_x";
            case input_code::pad_y: return "pad_y";
            case input_code::pad_left_trigger: return "pad_left_trigger";
            case input_code::pad_right_trigger: return "pad_right_trigger";
            case input_code::pad_left_bumper: return "pad_left_bumper";
            case input_code::pad_right_bumper: return "pad_right_bumper";
            case input_code::pad_back: return "pad_back";
            case input_code::pad_start: return "pad_start";
        }

        return "unknown";
    }
}

namespace std {
    template <>
    struct hash<multi_input::input_code> {
        using argument_type = multi_input::input_code;
        using result_type = std::hash<int>::result_type;

        result_type operator()(const argument_type& s) const {
            return std::hash<int>{}(static_cast<int>(s));
        }
    };
}
