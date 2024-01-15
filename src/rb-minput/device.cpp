// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#include "device.hpp"
#include "device_event.hpp"
#include "context.hpp"

namespace multi_input {
    device::device(context* ctx, device_id id) :
        m_ctx(ctx), m_id(id), m_meta(), m_axes(), m_is_usable(true)
    {
    }

    device::~device() {
    }

    const std::string& device::get_name() const {
        return m_meta.m_name;
    }

    device_meta& device::get_meta() {
        return m_meta;
    }

    device_id device::get_id() const {
        return m_id;
    }

    bool device::is_usable() const {
        return m_is_usable;
    }

    void device::set_usable(bool usable) {
        auto changed = m_is_usable != usable;
        m_is_usable = usable;

        if (changed) {
            m_ctx->notify_device(m_id, usable ? device_event::usable : device_event::unusable);
        }
    }

    bool device::can_vibrate() const {
        return false;
    }

    bool device::vibrate(int duration, float left, float right) {
        return false;
    }

    virtual_axis* device::get_axis(input_code code) {
        if (code == input_code::none) {
            return nullptr;
        }

        auto it = m_axes.find(code);
        if (it == m_axes.end()) {
            return nullptr;
        } else {
            return &it->second;
        }
    }

    virtual_axis* device::add_axis(input_code code) {
        if (code == input_code::none) {
            return nullptr;
        }

        auto pair = m_axes.emplace(code, virtual_axis{});
        auto it = pair.first;
        return &it->second;
    }

    size_t device::get_axis_count() const {
        return m_axes.size();
    }

    std::vector<input_code> device::get_axis_codes() {
        std::vector<input_code> codes{};
        codes.reserve(m_axes.size());

        for (auto&& pair : m_axes) {
            codes.emplace_back(pair.first);
        }

        return codes;
    }

    void device::reset() {
        for (auto&& pair : m_axes) {
            auto&& axis = pair.second;
            axis.set(0);
            axis.commit();
        }
    }

    void device::commit() {
        for (auto&& pair : m_axes) {
            auto&& axis = pair.second;
            axis.commit();
        }
    }
}
