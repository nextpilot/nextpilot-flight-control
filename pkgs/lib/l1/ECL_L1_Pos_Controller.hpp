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
 * @file ecl_l1_pos_control.h
 * Implementation of L1 position control.
 *
 *
 * Acknowledgements and References:
 *
 *    This implementation has been built for PX4 based on the original
 *    publication from [1] and does include a lot of the ideas (not code)
 *    from [2].
 *
 *
 *    [1] S. Park, J. Deyst, and J. P. How, "A New Nonlinear Guidance Logic for Trajectory Tracking,"
 *    Proceedings of the AIAA Guidance, Navigation and Control
 *    Conference, Aug 2004. AIAA-2004-4900.
 *
 *    [2] Paul Riseborough, Brandon Jones and Andrew Tridgell, L1 control for APM. Aug 2013.
 *     - Explicit control over frequency and damping
 *     - Explicit control over track capture angle
 *     - Ability to use loiter radius smaller than L1 length
 *     - Modified to use PD control for circle tracking to enable loiter radius less than L1 length
 *     - Modified to enable period and damping of guidance loop to be set explicitly
 *     - Modified to provide explicit control over capture angle
 *
 */

#ifndef ECL_L1_POS_CONTROLLER_H
#define ECL_L1_POS_CONTROLLER_H

#include <matrix/math.hpp>
#include <mathlib/mathlib.h>

/**
 * L1 Nonlinear Guidance Logic
 */
class ECL_L1_Pos_Controller
{
public:
	/**
	 * The current target bearing
	 *
	 * @return bearing angle (-pi..pi, in NED frame)
	 */
	float nav_bearing() { return matrix::wrap_pi(_nav_bearing); }

	/**
	 * Get lateral acceleration demand.
	 *
	 * @return Lateral acceleration in m/s^2
	 */
	float nav_lateral_acceleration_demand() { return _lateral_accel; }

	/**
	 * Bearing from aircraft to current target.
	 *
	 * @return bearing angle (-pi..pi, in NED frame)
	 */
	float target_bearing() { return _target_bearing; }

	/**
	 * Get the current crosstrack error.
	 *
	 * @return Crosstrack error in meters.
	 */
	float crosstrack_error() { return _crosstrack_error; }

	/**
	 * Navigate between two waypoints
	 *
	 * Calling this function with two waypoints results in the
	 * control outputs to fly to the line segment defined by
	 * the points and once captured following the line segment.
	 * This follows the logic in [1].
	 *
	 * @return sets _lateral_accel setpoint
	 */
	void navigate_waypoints(const matrix::Vector2f &vector_A, const matrix::Vector2f &vector_B,
				const matrix::Vector2f &vector_curr_position, const matrix::Vector2f &ground_speed);

	/**
	 * Set the L1 period.
	 */
	void set_l1_period(float period);

	/**
	 * Set the L1 damping factor.
	 *
	 * The original publication recommends a default of sqrt(2) / 2 = 0.707
	 */
	void set_l1_damping(float damping);

private:

	float _lateral_accel{0.0f};		///< Lateral acceleration setpoint in m/s^2
	float _L1_distance{20.0f};		///< L1 lead distance, defined by period and damping
	float _nav_bearing{0.0f};		///< bearing to L1 reference point
	float _crosstrack_error{0.0f};	///< crosstrack error in meters
	float _target_bearing{0.0f};		///< the heading setpoint

	float _L1_period{25.0f};		///< L1 tracking period in seconds
	float _L1_damping{0.75f};		///< L1 damping ratio
	float _L1_ratio{5.0f};		///< L1 ratio for navigation
	float _K_L1{2.0f};			///< L1 control gain for _L1_damping
	float _heading_omega{1.0f};		///< Normalized frequency
};


#endif /* ECL_L1_POS_CONTROLLER_H */
