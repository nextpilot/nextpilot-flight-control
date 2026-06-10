

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
 * @file BerzierN.hpp
 *
 * @author Julian Kent <julian@auterion.com>
 *
 * N-order Bezier library designed for time-aware trajectory tracking
 */

#pragma once
#include <matrix/math.hpp>

namespace bezier
{

/*
 * Calculates the location and velocity with respect to T on a given bezier curve of any order.
 *
 */
bool calculateBezierPosVel(const matrix::Vector3f *positions, int N, float t,
			   matrix::Vector3f &position, matrix::Vector3f &velocity);

/*
 * Calculates the position, velocity and acceleration with respect to T on a given bezier curve of any order.
 *
 */
bool calculateBezierPosVelAcc(const matrix::Vector3f *positions, int N, float t,
			      matrix::Vector3f &position, matrix::Vector3f &velocity, matrix::Vector3f &acceleration);

/*
 * Calculates the position and velocity of yaw with respect to t on a bezier curve.
 * All yaw setpoints are wrapped relative to the starting yaw.
 *
 */
bool calculateBezierYaw(const float *setpoints, int N, float t, float &yaw_setpoint, float &yaw_vel_setpoint);

/*
 * Calculates the fraction between the begin and end time which can be used for fast bezier curve lookups
 */
bool calculateT(int64_t start_time, int64_t end_time, int64_t now, float &T);

}
