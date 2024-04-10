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
 * @file ObstacleAvoidance_dummy.hpp
 * This is a dummy class to reduce flash space for when obstacle avoidance is not required
 *
 * @author Julian Kent
 */

#pragma once

#include <defines.h>
#include <commander/px4_custom_mode.h>


#include <matrix/matrix/math.hpp>


class ObstacleAvoidance
{
public:
	ObstacleAvoidance(void *) {} // takes void* argument to be compatible with ModuleParams constructor


	void injectAvoidanceSetpoints(matrix::Vector3f &pos_sp, matrix::Vector3f &vel_sp, float &yaw_sp,
				      float &yaw_speed_sp)
	{
		notify_dummy();
	};

	void updateAvoidanceDesiredWaypoints(const matrix::Vector3f &curr_wp, const float curr_yaw,
					     const float curr_yawspeed,
					     const matrix::Vector3f &next_wp, const float next_yaw, const float next_yawspeed, const bool ext_yaw_active,
					     const int wp_type)
	{
		notify_dummy();
	};

	void updateAvoidanceDesiredSetpoints(const matrix::Vector3f &pos_sp, const matrix::Vector3f &vel_sp,
					     const int type)
	{
		notify_dummy();
	}


	void checkAvoidanceProgress(const matrix::Vector3f &pos, const matrix::Vector3f &prev_wp,
				    float target_acceptance_radius, const matrix::Vector2f &closest_pt)
	{
		notify_dummy();
	};

protected:

	bool _logged_error = false;
	void notify_dummy()
	{
		if (!_logged_error) {
			PX4_ERR("Dummy avoidance library called!");
			_logged_error = true;
		}
	}
};
