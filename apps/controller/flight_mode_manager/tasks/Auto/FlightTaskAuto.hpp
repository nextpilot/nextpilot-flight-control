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
 * @file FlightTaskAuto.hpp
 *
 * Map from global triplet to local quadruple.
 */

#pragma once

#include "FlightTask.hpp"
#include <uORB/topics/position_setpoint_triplet.h>
#include <uORB/topics/position_setpoint.h>
#include <uORB/topics/home_position.h>
#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/vehicle_status.h>
#include <geo/geo.h>
#include <weather_vane/WeatherVane.hpp>
#include <mathlib/math/filter/AlphaFilter.hpp>
#include <motion_planning/PositionSmoothing.hpp>
#include "Sticks.hpp"
#include "StickAccelerationXY.hpp"
#include "StickYaw.hpp"

// TODO: make this switchable in the board config, like a module
#if CONSTRAINED_FLASH
#   include <avoidance/ObstacleAvoidance_dummy.hpp>
#else
#   include <avoidance/ObstacleAvoidance.hpp>
#endif

/**
 * This enum has to agree with position_setpoint_s type definition
 * The only reason for not using the struct position_setpoint is because
 * of the size
 */
enum class WaypointType : int {
    position = position_setpoint_s::SETPOINT_TYPE_POSITION,
    velocity = position_setpoint_s::SETPOINT_TYPE_VELOCITY,
    loiter   = position_setpoint_s::SETPOINT_TYPE_LOITER,
    takeoff  = position_setpoint_s::SETPOINT_TYPE_TAKEOFF,
    land     = position_setpoint_s::SETPOINT_TYPE_LAND,
    idle     = position_setpoint_s::SETPOINT_TYPE_IDLE
};

enum class State {
    offtrack,         /**< Vehicle is more than cruise speed away from track */
    target_behind,    /**< Vehicle is in front of target. */
    previous_infront, /**< Vehilce is behind previous waypoint.*/
    none              /**< Vehicle is in normal tracking mode from triplet previous to triplet target */
};

class FlightTaskAuto : public FlightTask {
public:
    FlightTaskAuto()          = default;
    virtual ~FlightTaskAuto() = default;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;
    void reActivate() override;
    bool updateInitialize() override;
    bool update() override;

    void overrideCruiseSpeed(const float cruise_speed_m_s) override;

protected:
    /**< Depending on state of vehicle, the internal waypoints might differ from target (for instance if offtrack). */
    void _updateInternalWaypoints();

    /**< Computes and sets heading a 2D vector */
    bool _compute_heading_from_2D_vector(float &heading, matrix::Vector2f v);

    /** Reset position or velocity setpoints in case of EKF reset event */
    void _ekfResetHandlerPositionXY(const matrix::Vector2f &delta_xy) override;
    void _ekfResetHandlerVelocityXY(const matrix::Vector2f &delta_vxy) override;
    void _ekfResetHandlerPositionZ(float delta_z) override;
    void _ekfResetHandlerVelocityZ(float delta_vz) override;
    void _ekfResetHandlerHeading(float delta_psi) override;

    void _checkEmergencyBraking();
    bool _generateHeadingAlongTraj(); /**< Generates heading along trajectory. */
    bool isTargetModified() const;
    void _updateTrajConstraints();

    void rcHelpModifyYaw(float &yaw_sp);

    /** determines when to trigger a takeoff (ignored in flight) */
    bool _checkTakeoff() override {
        return _want_takeoff;
    };

    void _prepareLandSetpoints();
    bool _highEnoughForLandingGear();           /**< Checks if gears can be lowered. */

    void updateParams() override;               /**< See ModuleParam class */

    matrix::Vector3f _prev_prev_wp{};           /**< Pre-previous waypoint (local frame). This will be used for smoothing trajectories -> not used yet. */
    matrix::Vector3f _prev_wp{};                /**< Previous waypoint  (local frame). If no previous triplet is available, the prev_wp is set to current position. */
    bool             _prev_was_valid{false};
    matrix::Vector3f _target{};                 /**< Target waypoint  (local frame).*/
    matrix::Vector3f _next_wp{};                /**< The next waypoint after target (local frame). If no next setpoint is available, next is set to target. */
    bool             _next_was_valid{false};
    float            _mc_cruise_speed{NAN};     /**< Requested cruise speed. If not valid, default cruise speed is used. */
    WaypointType     _type{WaypointType::idle}; /**< Type of current target triplet. */

    uORB::SubscriptionData<home_position_s>  _sub_home_position{ORB_ID(home_position)};
    uORB::SubscriptionData<vehicle_status_s> _sub_vehicle_status{ORB_ID(vehicle_status)};

    State _current_state{State::none};
    float _target_acceptance_radius{0.0f}; /**< Acceptances radius of the target */

    float              _yaw_sp_prev{NAN};
    AlphaFilter<float> _yawspeed_filter;
    bool               _yaw_sp_aligned{false};

    ObstacleAvoidance _obstacle_avoidance{this}; /**< class adjusting setpoints according to external avoidance module's input */

    PositionSmoothing   _position_smoothing;
    Vector3f            _unsmoothed_velocity_setpoint;
    Sticks              _sticks{this};
    StickAccelerationXY _stick_acceleration_xy{this};
    StickYaw            _stick_yaw{this};
    matrix::Vector3f    _land_position;
    float               _land_heading;
    WaypointType        _type_previous{WaypointType::idle}; /**< Previous type of current target triplet. */
    bool                _is_emergency_braking_active{false};
    bool                _want_takeoff{false};

    DEFINE_PARAMETERS_CUSTOM_PARENT(FlightTask,
                                    (ParamFloat<params_id::MPC_XY_CRUISE>)_param_mpc_xy_cruise,
                                    (ParamFloat<params_id::NAV_MC_ALT_RAD>)_param_nav_mc_alt_rad, // vertical acceptance radius at which waypoints are updated
                                    (ParamInt<params_id::MPC_YAW_MODE>)_param_mpc_yaw_mode,       // defines how heading is executed,
                                    (ParamInt<params_id::COM_OBS_AVOID>)_param_com_obs_avoid,     // obstacle avoidance active
                                    (ParamFloat<params_id::MPC_YAWRAUTO_MAX>)_param_mpc_yawrauto_max,
                                    (ParamFloat<params_id::MIS_YAW_ERR>)_param_mis_yaw_err,       // yaw-error threshold
                                    (ParamFloat<params_id::MPC_ACC_HOR>)_param_mpc_acc_hor,       // acceleration in flight
                                    (ParamFloat<params_id::MPC_ACC_UP_MAX>)_param_mpc_acc_up_max,
                                    (ParamFloat<params_id::MPC_ACC_DOWN_MAX>)_param_mpc_acc_down_max,
                                    (ParamFloat<params_id::MPC_JERK_AUTO>)_param_mpc_jerk_auto,
                                    (ParamFloat<params_id::MPC_XY_TRAJ_P>)_param_mpc_xy_traj_p,
                                    (ParamFloat<params_id::MPC_XY_ERR_MAX>)_param_mpc_xy_err_max,
                                    (ParamFloat<params_id::MPC_LAND_SPEED>)_param_mpc_land_speed,
                                    (ParamFloat<params_id::MPC_LAND_CRWL>)_param_mpc_land_crawl_speed,
                                    (ParamInt<params_id::MPC_LAND_RC_HELP>)_param_mpc_land_rc_help,
                                    (ParamFloat<params_id::MPC_LAND_RADIUS>)_param_mpc_land_radius,
                                    (ParamFloat<params_id::MPC_LAND_ALT1>)_param_mpc_land_alt1, // altitude at which we start ramping down speed
                                    (ParamFloat<params_id::MPC_LAND_ALT2>)_param_mpc_land_alt2, // altitude at which we descend at land speed
                                    (ParamFloat<params_id::MPC_LAND_ALT3>)_param_mpc_land_alt3, // altitude where we switch to crawl speed, if LIDAR available
                                    (ParamFloat<params_id::MPC_Z_V_AUTO_UP>)_param_mpc_z_v_auto_up,
                                    (ParamFloat<params_id::MPC_Z_V_AUTO_DN>)_param_mpc_z_v_auto_dn,
                                    (ParamFloat<params_id::MPC_TKO_SPEED>)_param_mpc_tko_speed,
                                    (ParamFloat<params_id::MPC_TKO_RAMP_T>)_param_mpc_tko_ramp_t // time constant for smooth takeoff ramp
    );

private:
    matrix::Vector2f _lock_position_xy{NAN, NAN}; /**< if no valid triplet is received, lock positition to current position */
    bool             _yaw_lock{false};            /**< if within acceptance radius, lock yaw to current yaw */

    uORB::SubscriptionData<position_setpoint_triplet_s> _sub_triplet_setpoint{ORB_ID(position_setpoint_triplet)};

    matrix::Vector3f _triplet_target;                /**< current triplet from navigator which may differ from the intenal one (_target) depending on the vehicle state. */
    matrix::Vector3f _triplet_prev_wp;               /**< previous triplet from navigator which may differ from the intenal one (_prev_wp) depending on the vehicle state.*/
    matrix::Vector3f _triplet_next_wp;               /**< next triplet from navigator which may differ from the intenal one (_next_wp) depending on the vehicle state.*/
    matrix::Vector3f _closest_pt;                    /**< closest point to the vehicle position on the line previous - target */

    hrt_abstime _time_last_cruise_speed_override{0}; ///< timestamp the cruise speed was last time overridden using DO_CHANGE_SPEED

    MapProjection _reference_position{};             /**< Class used to project lat/lon setpoint into local frame. */
    float         _reference_altitude{NAN};          /**< Altitude relative to ground. */
    hrt_abstime   _time_stamp_reference{0};          /**< time stamp when last reference update occured. */

    WeatherVane _weathervane{this};                  /**< weathervane library, used to implement a yaw control law that turns the vehicle nose into the wind */

    matrix::Vector3f _initial_land_position;

    void  _limitYawRate();                          /**< Limits the rate of change of the yaw setpoint. */
    bool  _evaluateTriplets();                      /**< Checks and sets triplets. */
    bool  _isFinite(const position_setpoint_s &sp); /**< Checks if all waypoint triplets are finite. */
    bool  _evaluateGlobalReference();               /**< Check is global reference is available. */
    State _getCurrentState();                       /**< Computes the current vehicle state based on the vehicle position and navigator triplets. */
    void  _set_heading_from_mode();                 /**< @see  MPC_YAW_MODE */
};
