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

#include "output.h"

#include <uORB/Publication.hpp>
#include <uORB/topics/gimbal_controls.h>
#include <uORB/topics/gimbal_device_attitude_status.h>

namespace gimbal {

class OutputRC : public OutputBase {
public:
    OutputRC() = delete;
    explicit OutputRC(const Parameters &parameters);
    virtual ~OutputRC() = default;

    virtual void update(const ControlData &control_data, bool new_setpoints, uint8_t &gimbal_device_id);
    virtual void print_status() const;

private:
    void _stream_device_attitude_status();

    uORB::Publication<gimbal_controls_s>               _gimbal_controls_pub{ORB_ID(gimbal_controls)};
    uORB::Publication<gimbal_device_attitude_status_s> _attitude_status_pub{ORB_ID(gimbal_device_attitude_status)};
};

} /* namespace gimbal */
