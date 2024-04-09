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
 * @file AttitudeControlMath.hpp
 */

#pragma once

#include <matrix/matrix/math.hpp>

namespace AttitudeControlMath {
/**
 * Rotate a tilt quaternion (without yaw rotation) such that when rotated by a yaw setpoint
 * the resulting tilt is the same as if it was rotated by the current yaw of the vehicle
 * @param q_sp_tilt pure tilt quaternion (yaw = 0) that needs to be corrected
 * @param q_att current attitude of the vehicle
 * @param q_sp_yaw pure yaw quaternion of the desired yaw setpoint
 */
void inline correctTiltSetpointForYawError(matrix::Quatf &q_sp_tilt, const matrix::Quatf &q_att,
                                           const matrix::Quatf &q_sp_yaw) {
    const matrix::Vector3f z_unit(0.f, 0.f, 1.f);

    // Extract yaw from the current attitude
    const matrix::Vector3f att_z = q_att.dcm_z();
    const matrix::Quatf    q_tilt(z_unit, att_z);
    const matrix::Quatf    q_yaw = q_tilt.inversed() * q_att; // This is not euler yaw

    // Find the quaternion that creates a tilt aligned with the body frame
    // when rotated by the yaw setpoint
    // To do so, solve q_yaw * q_tilt_ne = q_sp_yaw * q_sp_rp_compensated
    const matrix::Quatf q_sp_rp_compensated = q_sp_yaw.inversed() * q_yaw * q_sp_tilt;

    // Extract the corrected tilt
    const matrix::Vector3f att_sp_z = q_sp_rp_compensated.dcm_z();
    q_sp_tilt                       = matrix::Quatf(z_unit, att_sp_z);
}
} // namespace AttitudeControlMath
