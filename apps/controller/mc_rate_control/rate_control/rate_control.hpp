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
 * @file rate_control.hpp
 *
 * PID 3 axis angular rate / angular velocity control.
 */

#pragma once

#include <matrix/matrix/math.hpp>

#include <mathlib/mathlib.h>
#include <uORB/topics/rate_ctrl_status.h>

class RateControl {
public:
    RateControl()  = default;
    ~RateControl() = default;

    /**
     * Set the rate control gains
     * @param P 3D vector of proportional gains for body x,y,z axis
     * @param I 3D vector of integral gains
     * @param D 3D vector of derivative gains
     */
    void setGains(const matrix::Vector3f &P, const matrix::Vector3f &I, const matrix::Vector3f &D);

    /**
     * Set the mximum absolute value of the integrator for all axes
     * @param integrator_limit limit value for all axes x, y, z
     */
    void setIntegratorLimit(const matrix::Vector3f &integrator_limit) {
        _lim_int = integrator_limit;
    };

    /**
     * Set direct rate to torque feed forward gain
     * @see _gain_ff
     * @param FF 3D vector of feed forward gains for body x,y,z axis
     */
    void setFeedForwardGain(const matrix::Vector3f &FF) {
        _gain_ff = FF;
    };

    /**
     * Set saturation status
     * @param control saturation vector from control allocator
     */
    void setSaturationStatus(const matrix::Vector3<bool> &saturation_positive,
                             const matrix::Vector3<bool> &saturation_negative);

    /**
     * Set individual saturation flags
     * @param axis 0 roll, 1 pitch, 2 yaw
     * @param is_saturated value to update the flag with
     */
    void setPositiveSaturationFlag(size_t axis, bool is_saturated);
    void setNegativeSaturationFlag(size_t axis, bool is_saturated);

    /**
     * Run one control loop cycle calculation
     * @param rate estimation of the current vehicle angular rate
     * @param rate_sp desired vehicle angular rate setpoint
     * @param dt desired vehicle angular rate setpoint
     * @return [-1,1] normalized torque vector to apply to the vehicle
     */
    matrix::Vector3f update(const matrix::Vector3f &rate, const matrix::Vector3f &rate_sp,
                            const matrix::Vector3f &angular_accel, const float dt, const bool landed);

    /**
     * Set the integral term to 0 to prevent windup
     * @see _rate_int
     */
    void resetIntegral() {
        _rate_int.zero();
    }

    /**
     * Get status message of controller for logging/debugging
     * @param rate_ctrl_status status message to fill with internal states
     */
    void getRateControlStatus(rate_ctrl_status_s &rate_ctrl_status);

private:
    void updateIntegral(matrix::Vector3f &rate_error, const float dt);

    // Gains
    matrix::Vector3f _gain_p;  ///< rate control proportional gain for all axes x, y, z
    matrix::Vector3f _gain_i;  ///< rate control integral gain
    matrix::Vector3f _gain_d;  ///< rate control derivative gain
    matrix::Vector3f _lim_int; ///< integrator term maximum absolute value
    matrix::Vector3f _gain_ff; ///< direct rate to torque feed forward gain only useful for helicopters

    // States
    matrix::Vector3f _rate_int; ///< integral term of the rate controller

    // Feedback from control allocation
    matrix::Vector<bool, 3> _control_allocator_saturation_negative;
    matrix::Vector<bool, 3> _control_allocator_saturation_positive;
};
