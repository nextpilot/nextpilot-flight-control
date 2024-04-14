/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include "input.h"
#include "gimbal_params.h"
#include <uORB/topics/manual_control_setpoint.h>

namespace gimbal {

class InputRC : public InputBase {
public:
    InputRC() = delete;
    explicit InputRC(Parameters &parameters);

    virtual ~InputRC();

    virtual void print_status() const;

    virtual UpdateResult update(unsigned int timeout_ms, ControlData &control_data, bool already_active);

    virtual int initialize();

private:
    virtual UpdateResult _read_control_data_from_subscription(ControlData &control_data, bool already_active);
    float                _get_aux_value(const manual_control_setpoint_s &manual_control_setpoint, int channel_idx);

    int _manual_control_setpoint_sub{-1};

    float _last_set_aux_values[3]{};
};

} /* namespace gimbal */
