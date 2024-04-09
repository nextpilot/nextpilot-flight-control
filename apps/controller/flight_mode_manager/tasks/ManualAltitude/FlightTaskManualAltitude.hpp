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
 * @file FlightManualAltitude.hpp
 *
 * Flight task for manual controlled altitude.
 */

#pragma once

#include "FlightTask.hpp"
#include <lib/mathlib/math/filter/AlphaFilter.hpp>
#include "Sticks.hpp"
#include "StickTiltXY.hpp"
#include "StickYaw.hpp"
#include <uORB/Subscription.hpp>

class FlightTaskManualAltitude : public FlightTask {
public:
    FlightTaskManualAltitude()          = default;
    virtual ~FlightTaskManualAltitude() = default;
    bool activate(const trajectory_setpoint_s &last_setpoint) override;
    bool updateInitialize() override;
    bool update() override;

protected:
    void         _updateHeadingSetpoints();                         /**< sets yaw or yaw speed */
    void         _ekfResetHandlerHeading(float delta_psi) override; /**< adjust heading setpoint in case of EKF reset event */
    virtual void _updateSetpoints();                                /**< updates all setpoints */
    virtual void _scaleSticks();                                    /**< scales sticks to velocity in z */
    bool         _checkTakeoff() override;
    void         _updateConstraintsFromEstimator();

    /**
     *  Check and sets for position lock.
     *  If sticks are at center position, the vehicle
     *  will exit velocity control and enter position control.
     */
    void _updateAltitudeLock();

    Sticks      _sticks{this};
    StickTiltXY _stick_tilt_xy{this};
    StickYaw    _stick_yaw{this};

    bool _sticks_data_required = true; ///< let inherited task-class define if it depends on stick data
    bool _terrain_hold{false};         /**< true when vehicle is controlling height above a static ground position */

    DEFINE_PARAMETERS_CUSTOM_PARENT(FlightTask,
                                    (ParamFloat<params_id::MPC_HOLD_MAX_Z>)_param_mpc_hold_max_z,
                                    (ParamInt<params_id::MPC_ALT_MODE>)_param_mpc_alt_mode,
                                    (ParamFloat<params_id::MPC_HOLD_MAX_XY>)_param_mpc_hold_max_xy,
                                    (ParamFloat<params_id::MPC_Z_P>)_param_mpc_z_p,             /**< position controller altitude propotional gain */
                                    (ParamFloat<params_id::MPC_LAND_ALT1>)_param_mpc_land_alt1, /**< altitude at which to start downwards slowdown */
                                    (ParamFloat<params_id::MPC_LAND_ALT2>)_param_mpc_land_alt2, /**< altitude below which to land with land speed */
                                    (ParamFloat<params_id::MPC_LAND_SPEED>)
                                        _param_mpc_land_speed, /**< desired downwards speed when approaching the ground */
                                    (ParamFloat<params_id::MPC_TKO_SPEED>)
                                        _param_mpc_tko_speed /**< desired upwards speed when still close to the ground */
    )
private:
    bool _isYawInput();
    void _unlockYaw();
    void _lockYaw();

    /**
     * Terrain following.
     * During terrain following, the position setpoint is adjusted
     * such that the distance to ground is kept constant.
     * @param apply_brake is true if user wants to break
     * @param stopped is true if vehicle has stopped moving in D-direction
     */
    void _terrainFollowing(bool apply_brake, bool stopped);

    /**
     * Minimum Altitude during range sensor operation.
     * If a range sensor is used for altitude estimates, for
     * best operation a minimum altitude is required. The minimum
     * altitude is only enforced during altitude lock.
     */
    void _respectMinAltitude();

    void _respectMaxAltitude();

    /**
     * Sets downwards velocity constraint based on the distance to ground.
     * To ensure a slowdown to land speed before hitting the ground.
     */
    void _respectGroundSlowdown();

    void setGearAccordingToSwitch();

    uint8_t _reset_counter = 0; /**< counter for estimator resets in z-direction */

    float _min_distance_to_ground{(float)(-INFINITY)}; /**< min distance to ground constraint */
    float _max_distance_to_ground{(float)INFINITY};    /**< max distance to ground constraint */

    /**
     * Distance to ground during terrain following.
     * If user does not demand velocity change in D-direction and the vehcile
     * is in terrain-following mode, then height to ground will be locked to
     * _dist_to_ground_lock.
     */
    float _dist_to_ground_lock = NAN;
};
