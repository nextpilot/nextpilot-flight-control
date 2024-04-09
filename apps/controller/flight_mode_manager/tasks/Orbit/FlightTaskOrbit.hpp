/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file FlightTaskOrbit.hpp
 *
 * Flight task for orbiting in a circle around a target position
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include "FlightTaskManualAltitudeSmoothVel.hpp"
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/orbit_status.h>
#include <lib/slew_rate/SlewRateYaw.hpp>
#include <lib/motion_planning/PositionSmoothing.hpp>
#include <lib/motion_planning/VelocitySmoothing.hpp>

class FlightTaskOrbit : public FlightTaskManualAltitudeSmoothVel {
public:
    FlightTaskOrbit();
    virtual ~FlightTaskOrbit() = default;

    bool applyCommandParameters(const vehicle_command_s &command, bool &success) override;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;
    bool update() override;

protected:
    /**
     * Send out telemetry information for the log and MAVLink.
     * @return true on success, false on error
     */
    bool sendTelemetry();

private:
    /* TODO: Should be controlled by params */
    static constexpr float _radius_min                   = 1.f;
    static constexpr float _velocity_max                 = 10.f;
    static constexpr float _acceleration_max             = 2.f;
    static constexpr float _horizontal_acceptance_radius = 2.f;

    /**
     * Check the feasibility of orbit parameters with respect to
     * centripetal acceleration a = v^2 / r
     * @param radius desired radius
     * @param velocity desired velocity
     * @param acceleration maximal allowed acceleration
     * @return true on success, false if value not accepted
     */
    bool _accelerationValid(float radius, float velocity, float acceleration) const;

    /**
     * Checks if desired orbit params are feasible. If not,
     * params are modified such that it is possible
     * returns a feasible radius.
     * @param radius The radius of the orbit. May get modified
     * @param velocity The velocity of the orbit. May get modified
     * @return Feasible orbit params
     */
    void _sanitizeParams(float &radius, float &velocity) const;

    /**
     * @brief updates the trajectory boundaries from props
     */
    void _updateTrajectoryBoundaries();

    /**
     * @brief Checks if the current position is on the circle or not
     * Uses the params
     */
    bool _is_position_on_circle() const;

    /** Adjusts radius and speed according to stick input */
    void _adjustParametersByStick();
    /** generates setpoints to smoothly reach the closest point on the circle when starting from far away */
    void _generate_circle_approach_setpoints();
    /** generates xy setpoints to make the vehicle orbit */
    void _generate_circle_setpoints();
    /** generates yaw setpoints to control the vehicle's heading */
    void _generate_circle_yaw_setpoints();

    float            _orbit_velocity{};
    float            _orbit_radius{};
    matrix::Vector3f _center; /**< local frame coordinates of the center point */

    bool              _in_circle_approach = false;
    PositionSmoothing _position_smoothing;

    /** yaw behaviour during the orbit flight according to MAVLink's ORBIT_YAW_BEHAVIOUR enum */
    int                _yaw_behaviour = orbit_status_s::ORBIT_YAW_BEHAVIOUR_HOLD_FRONT_TO_CIRCLE_CENTER;
    bool               _started_clockwise{true};
    float              _initial_heading = 0.f; /**< the heading of the drone when the orbit command was issued */
    SlewRateYaw<float> _slew_rate_yaw;

    orb_advert_t                           _mavlink_log_pub{nullptr};
    uORB::PublicationMulti<orbit_status_s> _orbit_status_pub{ORB_ID(orbit_status)};

    DEFINE_PARAMETERS(
        (ParamFloat<px4::params::MC_ORBIT_RAD_MAX>)_param_mc_orbit_rad_max,
        (ParamFloat<px4::params::MPC_XY_CRUISE>)_param_mpc_xy_cruise, /**< cruise speed for circle approach */
        (ParamFloat<px4::params::MPC_YAWRAUTO_MAX>)_param_mpc_yawrauto_max,
        (ParamFloat<px4::params::MPC_XY_TRAJ_P>)_param_mpc_xy_traj_p,
        (ParamFloat<px4::params::NAV_MC_ALT_RAD>)
            _param_nav_mc_alt_rad, // vertical acceptance radius at which waypoints are updated
        (ParamFloat<px4::params::MPC_XY_ERR_MAX>)_param_mpc_xy_err_max,
        (ParamFloat<px4::params::MPC_ACC_HOR>)_param_mpc_acc_hor, // acceleration in flight
        (ParamFloat<px4::params::MPC_JERK_AUTO>)_param_mpc_jerk_auto,
        (ParamFloat<px4::params::MPC_ACC_UP_MAX>)_param_mpc_acc_up_max,
        (ParamFloat<px4::params::MPC_ACC_DOWN_MAX>)_param_mpc_acc_down_max,
        (ParamFloat<px4::params::MPC_Z_V_AUTO_UP>)_param_mpc_z_v_auto_up,
        (ParamFloat<px4::params::MPC_Z_V_AUTO_DN>)_param_mpc_z_v_auto_dn)
};
