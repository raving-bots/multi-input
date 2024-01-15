// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include <unordered_map>
#include <vector>

#include "utils.hpp"
#include "api_types.hpp"
#include "input_code.hpp"
#include "virtual_axis.hpp"

namespace multi_input {
    struct context;
    struct virtual_axis;

    struct device_meta {
        RB_COPYABLE(device_meta);
        device_meta() = default;

        void set_name(std::string name) {
            m_name = name;
        }

        void set_internal_id(std::string id) {
            m_internal_id = id;
        }

        void set_serial(std::string serial) {
            m_serial = serial;
        }

        void set_location(std::string location) {
            m_location_id = location;
        }

        void set_ids(int vid, int pid, int rev) {
            m_vendor_id = vid;
            m_product_id = pid;
            m_revision = rev;
        }

        const std::string& get_serial() const {
            return m_serial;
        }
    private:
        friend struct api_device;
        friend struct device;

        std::string m_name;
        std::string m_location_id;
        std::string m_internal_id;
        int m_vendor_id;
        int m_product_id;
        int m_revision;
        std::string m_serial;
    };

    struct device {
        RB_NON_MOVEABLE(device);
        virtual ~device();

        device_id get_id() const;
        const std::string& get_name() const;

        virtual bool is_usable() const;
        void set_usable(bool);

        virtual bool can_vibrate() const;
        virtual bool vibrate(int, float, float);
        virtual void commit();

        virtual_axis* get_axis(input_code);
        size_t get_axis_count() const;
        std::vector<input_code> get_axis_codes();
        void reset();
    protected:
        friend struct api_device;

        device(context*, device_id);
        virtual_axis* add_axis(input_code);
        device_meta& get_meta();

        context* m_ctx;
        device_id m_id;
        device_meta m_meta;
        std::unordered_map<input_code, virtual_axis> m_axes;
        bool m_is_usable;
    };

    struct api_device {
        device_id m_id;

        api_string m_name;
        api_string m_location_id;
        api_string m_internal_id;
        api_int m_vendor_id;
        api_int m_product_id;
        api_int m_revision;
        api_string m_serial;

        api_bool m_is_usable;
        api_bool m_can_vibrate;
        size_t m_axis_count;

        void set_from(device& d) {
            auto& meta = d.get_meta();

            m_id          = d.get_id();
            m_name        = meta.m_name.c_str();
            m_location_id = meta.m_location_id.c_str();
            m_internal_id = meta.m_internal_id.c_str();
            m_vendor_id   = meta.m_vendor_id;
            m_product_id  = meta.m_product_id;
            m_revision    = meta.m_revision;
            m_serial      = meta.m_serial.c_str();
            m_is_usable   = d.is_usable();
            m_can_vibrate = d.can_vibrate();
            m_axis_count  = d.get_axis_count();
        }
    };

    static_assert(std::is_pod<api_device>::value, "api_device must be a POD");
}
