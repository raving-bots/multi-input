// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "linux/evdev/evdev.hpp"

#include <sys/ioctl.h>

#include "linux/evdev/evdev_device.hpp"
#include "device.hpp"
#include "context.hpp"
#include "input_code.hpp"
#include "axis_utils.hpp"

namespace multi_input {
    namespace lnx {
        // TODO non xbox

        evdev_device::evdev_device(context* ctx, device_id id, evdev_handle&& handle) :
            device(ctx, id),
            m_handle(std::move(handle)), m_can_vibrate(), m_last_effect(-1)
        {
            auto handle_raw = m_handle.get();

            auto& meta = get_meta();
            meta.set_name(libevdev_get_name(handle_raw));
            meta.set_location(libevdev_get_phys(handle_raw));
            meta.set_internal_id(m_handle.get_path());
            meta.set_ids(
                libevdev_get_id_vendor(handle_raw),
                libevdev_get_id_product(handle_raw),
                libevdev_get_id_version(handle_raw)
            );
            meta.set_serial(m_handle.get_info().get("ID_SERIAL"));

            if (try_add_axis(EV_ABS, ABS_X)) {
                add_axis(input_code::pad_left_stick_left);
                add_axis(input_code::pad_left_stick_right);
            }

            if (try_add_axis(EV_ABS, ABS_Y)) {
                add_axis(input_code::pad_left_stick_up);
                add_axis(input_code::pad_left_stick_down);
            }

            if (try_add_axis(EV_ABS, ABS_RX)) {
                add_axis(input_code::pad_right_stick_left);
                add_axis(input_code::pad_right_stick_right);
            }

            if (try_add_axis(EV_ABS, ABS_RY)) {
                add_axis(input_code::pad_right_stick_up);
                add_axis(input_code::pad_right_stick_down);
            }

            if (try_add_axis(EV_KEY, BTN_TRIGGER_HAPPY1) && try_add_axis(EV_KEY, BTN_TRIGGER_HAPPY2)) {
                add_axis(input_code::pad_dpad_x);
            }

            if (try_add_axis(EV_KEY, BTN_TRIGGER_HAPPY3) && try_add_axis(EV_KEY, BTN_TRIGGER_HAPPY4)) {
                add_axis(input_code::pad_dpad_y);
            }

            try_add_axis(EV_ABS, ABS_Z);
            try_add_axis(EV_ABS, ABS_RZ);

            try_add_axis(EV_KEY, BTN_A);
            try_add_axis(EV_KEY, BTN_B);
            try_add_axis(EV_KEY, BTN_X);
            try_add_axis(EV_KEY, BTN_Y);
            try_add_axis(EV_KEY, BTN_THUMBL);
            try_add_axis(EV_KEY, BTN_THUMBR);
            try_add_axis(EV_KEY, BTN_TL);
            try_add_axis(EV_KEY, BTN_TR);
            try_add_axis(EV_KEY, BTN_SELECT);
            try_add_axis(EV_KEY, BTN_START);

            // TODO maybe other force-feedback types?
            m_can_vibrate = libevdev_has_event_code(m_handle.get(), EV_FF, FF_RUMBLE);
        }

        evdev_device::~evdev_device() {
        }

        bool evdev_device::try_add_axis(unsigned event_type, unsigned event_code) {
            auto handle = m_handle.get();

            if (!libevdev_has_event_code(handle, event_type, event_code)) {
                m_ctx->log_debug(
                    "evdev: #%1% doesn't support type %2% code %3%",
                    m_id,
                    libevdev_event_type_get_name(event_type),
                    libevdev_event_code_get_name(event_type, event_code)
                );
                return false;
            } else {
                auto axis_code = event_type == EV_KEY ? map_button_code(event_code) : map_axis_code(event_code);
                add_axis(axis_code);
                return true;
            }
        }

        void evdev_device::update(const input_event& event) {
            m_ctx->log_verbose(
                "evdev: device %1% event %2% code %3% value %4%",
                m_id,
                libevdev_event_type_get_name(event.type),
                libevdev_event_code_get_name(event.type, event.code),
                event.value
            );

            switch (event.type) {
                case EV_KEY:
                    update_button(event.code, event.value);
                    break;
                case EV_ABS:
                    update_axis(event.code, event.value);
                    break;
            }
        }

        input_code evdev_device::map_button_code(unsigned code) {
            switch (code) {
                case BTN_A:              return input_code::pad_a;
                case BTN_B:              return input_code::pad_b;
                case BTN_X:              return input_code::pad_x;
                case BTN_Y:              return input_code::pad_y;
                case BTN_TRIGGER_HAPPY1: return input_code::pad_dpad_left;
                case BTN_TRIGGER_HAPPY2: return input_code::pad_dpad_right;
                case BTN_TRIGGER_HAPPY3: return input_code::pad_dpad_up;
                case BTN_TRIGGER_HAPPY4: return input_code::pad_dpad_down;
                case BTN_THUMBL:         return input_code::pad_left_stick;
                case BTN_THUMBR:         return input_code::pad_right_stick;
                case BTN_TL:             return input_code::pad_left_bumper;
                case BTN_TR:             return input_code::pad_right_bumper;
                case BTN_SELECT:         return input_code::pad_back;
                case BTN_START:          return input_code::pad_start;
                default:                 return input_code::none;
            }
        }

        input_code evdev_device::map_axis_code(unsigned code) {
            switch (code) {
                case ABS_X:              return input_code::pad_left_stick_x;
                case ABS_Y:              return input_code::pad_left_stick_y;
                case ABS_Z:              return input_code::pad_left_trigger;
                case ABS_RX:             return input_code::pad_right_stick_x;
                case ABS_RY:             return input_code::pad_right_stick_y;
                case ABS_RZ:             return input_code::pad_right_trigger;
                default:                 return input_code::none;
            }
        }

        void evdev_device::update_button(unsigned code, int value) {
            auto axis_code = map_button_code(code);
            auto axis = get_axis(axis_code);

            if (axis_code == input_code::none) {
                return;
            }

            if (axis == nullptr) {
                m_ctx->log_warning(
                    u8"evdev: possible bug: got button code %1% (mapped %2%) but it wasn't added during discovery",
                    code, static_cast<int>(axis_code)
                );
                axis = add_axis(axis_code);
            }

            m_ctx->log_verbose(
                u8"evdev: %1%: axis %2% device %3%",
                value != 0 ? u8"button press" : u8"button release",
                static_cast<int>(axis_code), m_id
            );

            axis->set(value != 0 ? 1.0f : 0.0f);
        }

        void evdev_device::update_axis(unsigned code, int raw_value) {
            auto axis_code = map_axis_code(code);
            auto axis = get_axis(axis_code);

            if (axis_code == input_code::none) {
                return;
            }

            if (axis == nullptr) {
                m_ctx->log_warning(
                    u8"evdev: possible bug: got axis code %1% (mapped %2%) but it wasn't added during discovery",
                    code, static_cast<int>(axis_code)
                );
                axis = add_axis(axis_code);
            }

            auto value = map_value(code, raw_value);

            m_ctx->log_verbose(
                u8"evdev: axis %1% value %2% (mapped %3%) device %4%",
                static_cast<int>(axis_code), raw_value, value, m_id
            );

            axis->set(value);
        }

        // TODO non xbox, unify platforms
        float evdev_device::get_deadzone_for(unsigned code) {
            switch (code) {
                case ABS_X:
                case ABS_Y:
                    return deadzone_left_stick;
                case ABS_RX:
                case ABS_RY:
                    return deadzone_right_stick;
                case ABS_Z:
                case ABS_RZ:
                    return deadzone_trigger;
                default:
                    return 0;
            }
        }

        float evdev_device::map_value(unsigned code, float raw_value) {
            // reverse Y axis
            if (code == ABS_Y || code == ABS_RY) {
                raw_value = -raw_value;
            }

            // TODO configurable deadzones
            auto deadzone = get_deadzone_for(code);

            if (raw_value < 0) {
                auto abs_max = -libevdev_get_abs_minimum(m_handle.get(), code);
                return -apply_deadzone(-raw_value, abs_max, deadzone);
            } else {
                auto abs_max = libevdev_get_abs_maximum(m_handle.get(), code);
                return apply_deadzone(raw_value, abs_max, deadzone);
            }
        }

        void evdev_device::post_update() {
            // wireless Xbox controllers send us garbage packets
            // when they're first turned on while the application is running
            // (multiple axes will be non-zero even though nothing is physically pressed)
            // since user pressing and holding all controller buttons is rather unlikely
            // and also not very useful thing to support, we detect whether several buttons
            // are held and if so, reset our state
            static input_code buttons[] = {
                input_code::pad_a,
                input_code::pad_b,
                input_code::pad_x,
                input_code::pad_x,
                input_code::pad_dpad_left,
                input_code::pad_dpad_right,
                input_code::pad_dpad_up,
                input_code::pad_dpad_down,
                input_code::pad_left_stick,
                input_code::pad_right_stick,
                input_code::pad_left_bumper,
                input_code::pad_right_bumper,
                input_code::pad_back,
                input_code::pad_start
            };

            if (!is_usable()) {
                return;
            }

            int count = 0;
            for (auto code : buttons) {
                auto axis = get_axis(code);
                if (axis != nullptr && axis->get_next() != 0) {
                    ++count;
                }
            }

            // value picked arbitrarily
            if (count >= 6) {
                m_ctx->log_debug(u8"evdev: %2% buttons pressed on device %1%: this is probably garbage data, resetting internal state", m_id, count);
                reset();
            }
        }

        bool evdev_device::vibrate(int duration, float left, float right) {
            RB_TRACE_ENTER();

            if (!m_can_vibrate || !is_usable()) {
                RB_TRACE("cannot vibrate");
                return false;
            }

            int fd = get_handle().get_fd();
            m_ctx->log_debug("evdev: vibrating device %1% with force %2%/%3% for %4%ms", m_id, left, right, duration);

            if (m_last_effect != -1) {
                RB_TRACE("removing previous FF effect");
                m_ctx->log_debug("evdev: removing previous FF effect from device memory: %1%", m_last_effect);

                if (ioctl(fd, EVIOCRMFF, m_last_effect) == -1) {
                   throw_posix_error("evdev: failed to remove FF effect %1% from device fd %2%", m_last_effect, fd);
                }

                m_last_effect = -1;
            }

            RB_TRACE("creating new FF effect");

            ff_effect effect{};
            effect.type = FF_RUMBLE;
            effect.id = -1;
            effect.replay.length = duration;
            effect.u.rumble.strong_magnitude = static_cast<unsigned short>(left * 16384);
            effect.u.rumble.weak_magnitude = static_cast<unsigned short>(right * 65535);

            if (ioctl(fd, EVIOCSFF, &effect) == -1) {
                throw_posix_error("evdev: failed to upload new FF effect to device fd %1%", fd);
            }

            m_last_effect = effect.id;
            m_ctx->log_debug("evdev: created new FF_RUMBLE effect with id %1%", m_last_effect);

            input_event play{};
            play.type = EV_FF;
            play.code = m_last_effect;
            play.value = 3;

            if (write(fd, static_cast<const void*>(&play), sizeof(play)) == -1) {
                throw_posix_error("evdev: failed to start FF effect %1% on device fd %2%", m_last_effect, fd);
            }

            return true;
        }

        void evdev_device::commit() {
            derive_stick_pre_commit(*this);
            device::commit();
        }
    }
}
