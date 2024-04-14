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

#include "common.h"
#include "gimbal_params.h"
#include <drivers/drv_hrt.h>
#include <lib/geo/geo.h>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/mount_orientation.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_land_detected.h>

namespace gimbal {

class OutputBase {
public:
    OutputBase() = delete;
    explicit OutputBase(const Parameters &parameters);
    virtual ~OutputBase() = default;

    virtual void update(const ControlData &control_data, bool new_setpoints, uint8_t &gimbal_device_id) = 0;

    virtual void print_status() const = 0;

    void publish();

    void set_stabilize(bool roll_stabilize, bool pitch_stabilize, bool yaw_stabilize);

protected:
    float _calculate_pitch(double lon, double lat, float altitude,
                           const vehicle_global_position_s &global_position);

    MapProjection _projection_reference{}; ///< class to convert (lon, lat) to local [m]

    const Parameters &_parameters;

    /** set angle setpoints, speeds & stabilize flags */
    void _set_angle_setpoints(const ControlData &control_data);

    void _handle_position_update(const ControlData &control_data, bool force_update = false);

    float _q_setpoint[4]     = {NAN, NAN, NAN, NAN}; ///< can be NAN if not specifically set
    float _angle_velocity[3] = {NAN, NAN, NAN};      //< [rad/s], can be NAN if not specifically set

    bool _stabilize[3] = {false, false, false};

    // Pitch and role are by default aligned with the horizon.
    // Yaw follows the vehicle (not lock/absolute mode).
    bool _absolute_angle[3] = {true, true, false};

    /** calculate the _angle_outputs (with speed) and stabilize if needed */
    void _calculate_angle_output(const hrt_abstime &t);

    float       _angle_outputs[3] = {0.f, 0.f, 0.f}; ///< calculated output angles (roll, pitch, yaw) [rad]
    hrt_abstime _last_update;

private:
    uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription _vehicle_global_position_sub{ORB_ID(vehicle_global_position)};
    uORB::Subscription _vehicle_land_detected_sub{ORB_ID(vehicle_land_detected)};

    uORB::Publication<mount_orientation_s> _mount_orientation_pub{ORB_ID(mount_orientation)};

    bool _landed{true};
};

} /* namespace gimbal */
