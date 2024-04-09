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
 * @file AttitudeControl.hpp
 *
 * A quaternion based attitude controller.
 *
 * @author Matthias Grob	<maetugr@gmail.com>
 *
 * Publication documenting the implemented Quaternion Attitude Control:
 * Nonlinear Quadrocopter Attitude Control (2013)
 * by Dario Brescianini, Markus Hehn and Raffaello D'Andrea
 * Institute for Dynamic Systems and Control (IDSC), ETH Zurich
 *
 * https://www.research-collection.ethz.ch/bitstream/handle/20.500.11850/154099/eth-7387-01.pdf
 */

#pragma once

#include <matrix/matrix/math.hpp>
#include <mathlib/math/Limits.hpp>

class AttitudeControl {
public:
    AttitudeControl()  = default;
    ~AttitudeControl() = default;

    /**
     * Set proportional attitude control gain
     * @param proportional_gain 3D vector containing gains for roll, pitch, yaw
     * @param yaw_weight A fraction [0,1] deprioritizing yaw compared to roll and pitch
     */
    void setProportionalGain(const matrix::Vector3f &proportional_gain, const float yaw_weight);

    /**
     * Set hard limit for output rate setpoints
     * @param rate_limit [rad/s] 3D vector containing limits for roll, pitch, yaw
     */
    void setRateLimit(const matrix::Vector3f &rate_limit) {
        _rate_limit = rate_limit;
    }

    /**
     * Set a new attitude setpoint replacing the one tracked before
     * @param qd desired vehicle attitude setpoint
     * @param yawspeed_setpoint [rad/s] yaw feed forward angular rate in world frame
     */
    void setAttitudeSetpoint(const matrix::Quatf &qd, const float yawspeed_setpoint) {
        _attitude_setpoint_q = qd;
        _attitude_setpoint_q.normalize();
        _yawspeed_setpoint = yawspeed_setpoint;
    }

    /**
     * Adjust last known attitude setpoint by a delta rotation
     * Optional use to avoid glitches when attitude estimate reference e.g. heading changes.
     * @param q_delta delta rotation to apply
     */
    void adaptAttitudeSetpoint(const matrix::Quatf &q_delta) {
        _attitude_setpoint_q = q_delta * _attitude_setpoint_q;
        _attitude_setpoint_q.normalize();
    }

    /**
     * Run one control loop cycle calculation
     * @param q estimation of the current vehicle attitude unit quaternion
     * @return [rad/s] body frame 3D angular rate setpoint vector to be executed by the rate controller
     */
    matrix::Vector3f update(const matrix::Quatf &q) const;

private:
    matrix::Vector3f _proportional_gain;
    matrix::Vector3f _rate_limit;
    float            _yaw_w{0.f}; ///< yaw weight [0,1] to deprioritize caompared to roll and pitch

    matrix::Quatf _attitude_setpoint_q;    ///< latest known attitude setpoint e.g. from position control
    float         _yawspeed_setpoint{0.f}; ///< latest known yawspeed feed-forward setpoint
};
