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
 * @file ECL_L1_Pos_Controller.cpp
 * Implementation of L1 position control.
 * Authors and acknowledgements in header.
 *
 */

#include "ECL_L1_Pos_Controller.hpp"
#include <geo/geo.h>
#include <defines.h>
#include <float.h>

using matrix::Vector2f;

void
ECL_L1_Pos_Controller::navigate_waypoints(const Vector2f &vector_A, const Vector2f &vector_B,
		const Vector2f &vector_curr_position, const Vector2f &ground_speed_vector)
{
	/* this follows the logic presented in [1] */
	float eta = 0.0f;

	/* get the direction between the last (visited) and next waypoint */
	Vector2f vector_P_to_B = vector_B - vector_curr_position;
	Vector2f vector_P_to_B_unit = vector_P_to_B.normalized();
	_target_bearing = atan2f(vector_P_to_B_unit(1), vector_P_to_B_unit(0));

	/* enforce a minimum ground speed of 0.1 m/s to avoid singularities */
	float ground_speed = math::max(ground_speed_vector.length(), 0.1f);

	/* calculate the L1 length required for the desired period */
	_L1_distance = _L1_ratio * ground_speed;

	/* calculate vector from A to B */
	Vector2f vector_AB = vector_B - vector_A;

	/*
	 * check if waypoints are on top of each other. If yes,
	 * skip A and directly continue to B
	 */
	if (vector_AB.length() < 1.0e-6f) {
		vector_AB = vector_B - vector_curr_position;
	}

	vector_AB.normalize();

	/* calculate the vector from waypoint A to the aircraft */
	Vector2f vector_A_to_airplane = vector_curr_position - vector_A;

	/* calculate crosstrack error (output only) */
	_crosstrack_error = vector_AB % vector_A_to_airplane;

	/*
	 * If the current position is in a +-135 degree angle behind waypoint A
	 * and further away from A than the L1 distance, then A becomes the L1 point.
	 * If the aircraft is already between A and B normal L1 logic is applied.
	 */
	float distance_A_to_airplane = vector_A_to_airplane.length();
	float alongTrackDist = vector_A_to_airplane * vector_AB;

	/* estimate airplane position WRT to B */
	Vector2f vector_B_to_P = vector_curr_position - vector_B;
	Vector2f vector_B_to_P_unit = vector_B_to_P.normalized();

	/* calculate angle of airplane position vector relative to line) */

	// XXX this could probably also be based solely on the dot product
	float AB_to_BP_bearing = atan2f(vector_B_to_P_unit % vector_AB, vector_B_to_P_unit * vector_AB);

	/* extension from [2], fly directly to A */
	if (distance_A_to_airplane > _L1_distance && alongTrackDist / math::max(distance_A_to_airplane, 1.0f) < -0.7071f) {

		/* calculate eta to fly to waypoint A */

		/* unit vector from waypoint A to current position */
		Vector2f vector_A_to_airplane_unit = vector_A_to_airplane.normalized();

		/* velocity across / orthogonal to line */
		float xtrack_vel = ground_speed_vector % (-vector_A_to_airplane_unit);

		/* velocity along line */
		float ltrack_vel = ground_speed_vector * (-vector_A_to_airplane_unit);
		eta = atan2f(xtrack_vel, ltrack_vel);

		/* bearing from current position to L1 point */
		_nav_bearing = atan2f(-vector_A_to_airplane_unit(1), -vector_A_to_airplane_unit(0));

		/*
		 * If the AB vector and the vector from B to airplane point in the same
		 * direction, we have missed the waypoint. At +- 90 degrees we are just passing it.
		 */

	} else if (fabsf(AB_to_BP_bearing) < math::radians(100.0f)) {
		/*
		 * Extension, fly back to waypoint.
		 *
		 * This corner case is possible if the system was following
		 * the AB line from waypoint A to waypoint B, then is
		 * switched to manual mode (or otherwise misses the waypoint)
		 * and behind the waypoint continues to follow the AB line.
		 */

		/* calculate eta to fly to waypoint B */

		/* velocity across / orthogonal to line */
		float xtrack_vel = ground_speed_vector % (-vector_B_to_P_unit);

		/* velocity along line */
		float ltrack_vel = ground_speed_vector * (-vector_B_to_P_unit);
		eta = atan2f(xtrack_vel, ltrack_vel);

		/* bearing from current position to L1 point */
		_nav_bearing = atan2f(-vector_B_to_P_unit(1), -vector_B_to_P_unit(0));

	} else {
		/* calculate eta to fly along the line between A and B */

		/* velocity across / orthogonal to line */
		float xtrack_vel = ground_speed_vector % vector_AB;

		/* velocity along line */
		float ltrack_vel = ground_speed_vector * vector_AB;

		/* calculate eta2 (angle of velocity vector relative to line) */
		float eta2 = atan2f(xtrack_vel, ltrack_vel);

		/* calculate eta1 (angle to L1 point) */
		float xtrackErr = vector_A_to_airplane % vector_AB;
		float sine_eta1 = xtrackErr / math::max(_L1_distance, 0.1f);

		/* limit output to feasible values */
		sine_eta1 = math::constrain(sine_eta1, -1.0f, 1.0f);
		float eta1 = asinf(sine_eta1);
		eta = eta1 + eta2;

		/* bearing from current position to L1 point */
		_nav_bearing = atan2f(vector_AB(1), vector_AB(0)) + eta1;
	}

	/* limit angle to +-90 degrees */
	eta = math::constrain(eta, (-M_PI_F) / 2.0f, +M_PI_F / 2.0f);
	_lateral_accel = _K_L1 * ground_speed * ground_speed / _L1_distance * sinf(eta);

}

void ECL_L1_Pos_Controller::set_l1_period(float period)
{
	_L1_period = period;

	/* calculate the ratio introduced in [2] */
	_L1_ratio = 1.0f / M_PI_F * _L1_damping * _L1_period;

	/* calculate normalized frequency for heading tracking */
	_heading_omega = sqrtf(2.0f) * M_PI_F / _L1_period;
}

void ECL_L1_Pos_Controller::set_l1_damping(float damping)
{
	_L1_damping = damping;

	/* calculate the ratio introduced in [2] */
	_L1_ratio = 1.0f / M_PI_F * _L1_damping * _L1_period;

	/* calculate the L1 gain (following [2]) */
	_K_L1 = 4.0f * _L1_damping * _L1_damping;
}
