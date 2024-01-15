// SPDX-License-Identifier: Apache-2.0
// Copyright 2016-2024 Raving Bots

#pragma once

#include "api_types.hpp"

namespace multi_input {
    // basic api
    RB_API options* RB_APICALL_POST rb_minput_create_options();
    RB_API api_bool RB_APICALL_POST rb_minput_set_log_level(options*, log_level);
    RB_API api_bool RB_APICALL_POST rb_minput_set_stderr_log_sink(options*);
    RB_API api_bool RB_APICALL_POST rb_minput_set_custom_log_sink(options*, log_callback, user_data);
    RB_API api_bool RB_APICALL_POST rb_minput_set_device_callback(options*, device_callback, user_data);
    RB_API api_bool RB_APICALL_POST rb_minput_destroy_options(options*);

    RB_API context* RB_APICALL_POST rb_minput_create(options*);
    RB_API options* RB_APICALL_POST rb_minput_get_options(context*);
    RB_API api_bool RB_APICALL_POST rb_minput_set_options(context*, options*);
    RB_API api_bool RB_APICALL_POST rb_minput_destroy(context*);
    RB_API api_bool RB_APICALL_POST rb_minput_reset(context*);

    // events
    RB_API api_bool RB_APICALL_POST rb_minput_drain_events(context*);

    // device list
    RB_API enumeration* RB_APICALL_POST rb_minput_get_devices(context*);
    RB_API api_bool RB_APICALL_POST rb_minput_next_device(context*, enumeration*, api_device*);
    RB_API api_bool RB_APICALL_POST rb_minput_destroy_enumeration(context*, enumeration*);

    // device
    RB_API api_bool RB_APICALL_POST rb_minput_get_device(context*, device_id, api_device*);
    RB_API api_bool RB_APICALL_POST rb_minput_is_usable(context*, device_id);
    RB_API api_bool RB_APICALL_POST rb_minput_can_vibrate(context*, device_id);
    RB_API api_bool RB_APICALL_POST rb_minput_vibrate(context*, device_id, int, float, float);
    RB_API api_bool RB_APICALL_POST rb_minput_reset_device(context*, device_id);
    RB_API size_t RB_APICALL_POST rb_minput_get_axis_count(context*, device_id);
    RB_API api_bool RB_APICALL_POST rb_minput_get_axes(context*, device_id, input_code*, size_t);
    RB_API api_bool RB_APICALL_POST rb_minput_has_axis(context*, device_id, input_code);

    // virtual axes
    RB_API api_bool RB_APICALL_POST rb_minput_get_values(context*, device_id, input_code, float*, float*, float*);
    RB_API float RB_APICALL_POST rb_minput_get_value(context*, device_id, input_code);
    RB_API float RB_APICALL_POST rb_minput_get_previous(context*, device_id, input_code);
    RB_API float RB_APICALL_POST rb_minput_get_next(context*, device_id, input_code);
    RB_API api_bool RB_APICALL_POST rb_minput_set_value(context*, device_id, input_code, float);
    RB_API api_bool RB_APICALL_POST rb_minput_add_value(context*, device_id, input_code, float);
    RB_API api_bool RB_APICALL_POST rb_minput_commit_value(context*, device_id, input_code);
    RB_API api_bool RB_APICALL_POST rb_minput_find_first(context*, find_callback, user_data, input_code*, size_t, device_id*, input_code*);
}
