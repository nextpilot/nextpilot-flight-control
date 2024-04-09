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
 * @file Sticks.hpp
 *
 * Library abstracting stick input from manual_control_setpoint
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include <px4_platform_common/module_params.hpp>
#include <matrix/matrix/math.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/manual_control_setpoint.h>
#include <uORB/topics/failsafe_flags.h>

class Sticks : public ModuleParams {
public:
    Sticks(ModuleParams *parent);
    ~Sticks() = default;

    // Checks for updated manual control input & updates internal values
    bool checkAndUpdateStickInputs();

    bool isAvailable() {
        return _input_available;
    };

    // Position : 0 : pitch, 1 : roll, 2 : throttle, 3 : yaw
    const matrix::Vector4f &getPosition() {
        return _positions;
    }; // Raw stick position, no deadzone
    const matrix::Vector4f &getPositionExpo() {
        return _positions_expo;
    }; // Deadzone and expo applied

    // Helper functions to get stick values more intuitively
    float getRoll() const {
        return _positions(1);
    }
    float getRollExpo() const {
        return _positions_expo(1);
    }
    float getPitch() const {
        return _positions(0);
    }
    float getPitchExpo() const {
        return _positions_expo(0);
    }
    float getYaw() const {
        return _positions(3);
    }
    float getYawExpo() const {
        return _positions_expo(3);
    }
    float getThrottleZeroCentered() const {
        return -_positions(2);
    } // Convert Z-axis(down) command to Up-axis frame
    float getThrottleZeroCenteredExpo() const {
        return -_positions_expo(2);
    }
    const matrix::Vector2f getPitchRoll() {
        return _positions.slice<2, 1>(0, 0);
    }
    const matrix::Vector2f getPitchRollExpo() {
        return _positions_expo.slice<2, 1>(0, 0);
    }

    /**
     * Limit the the horizontal input from a square shaped joystick gimbal to a unit circle
     * @param v Vector containing x, y, z axis of the joystick gimbal. x, y get adjusted
     */
    static void limitStickUnitLengthXY(matrix::Vector2f &v);

    /**
     * Rotate horizontal component of joystick input into the vehicle body orientation
     * @param v Vector containing x, y, z axis of the joystick input.
     * @param yaw Current vehicle yaw heading
     * @param yaw_setpoint Current yaw setpoint if it's locked else NAN
     */
    static void rotateIntoHeadingFrameXY(matrix::Vector2f &v, const float yaw, const float yaw_setpoint);

private:
    bool             _input_available{false};
    matrix::Vector4f _positions;      ///< unmodified manual stick inputs
    matrix::Vector4f _positions_expo; ///< modified manual sticks using expo function

    uORB::Subscription _manual_control_setpoint_sub{ORB_ID(manual_control_setpoint)};
    uORB::Subscription _failsafe_flags_sub{ORB_ID(failsafe_flags)};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::MPC_HOLD_DZ>)_param_mpc_hold_dz,
        (ParamFloat<params_id::MPC_XY_MAN_EXPO>)_param_mpc_xy_man_expo,
        (ParamFloat<params_id::MPC_Z_MAN_EXPO>)_param_mpc_z_man_expo,
        (ParamFloat<params_id::MPC_YAW_EXPO>)_param_mpc_yaw_expo)
};
