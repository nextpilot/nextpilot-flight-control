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
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/gimbal_device_set_attitude.h>
#include <uORB/topics/gimbal_device_information.h>
#include <uORB/topics/gimbal_device_attitude_status.h>

namespace gimbal {
class OutputMavlinkV1 : public OutputBase {
public:
    OutputMavlinkV1(const Parameters &parameters);
    virtual ~OutputMavlinkV1() = default;

    virtual void update(const ControlData &control_data, bool new_setpoints, uint8_t &gimbal_device_id);

    virtual void print_status() const;

private:
    void                                               _stream_device_attitude_status();
    uORB::Publication<vehicle_command_s>               _gimbal_v1_command_pub{ORB_ID(gimbal_v1_command)};
    uORB::Publication<gimbal_device_attitude_status_s> _attitude_status_pub{ORB_ID(gimbal_device_attitude_status)};

    ControlData::Type _previous_control_data_type{ControlData::Type::Neutral};
};

class OutputMavlinkV2 : public OutputBase {
public:
    OutputMavlinkV2(const Parameters &parameters);
    virtual ~OutputMavlinkV2() = default;

    virtual void update(const ControlData &control_data, bool new_setpoints, uint8_t &gimbal_device_id);

    virtual void print_status() const;

private:
    void _publish_gimbal_device_set_attitude();
    void _request_gimbal_device_information();
    void _check_for_gimbal_device_information();

    uORB::Publication<gimbal_device_set_attitude_s> _gimbal_device_set_attitude_pub{ORB_ID(gimbal_device_set_attitude)};
    uORB::Subscription                              _gimbal_device_information_sub{ORB_ID(gimbal_device_information)};

    uint8_t     _gimbal_device_compid{0};
    hrt_abstime _last_gimbal_device_checked{0};
    bool        _gimbal_device_found{false};
};

} /* namespace gimbal */
