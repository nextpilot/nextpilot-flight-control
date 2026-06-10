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
 * @file terrain_estimator.h
 */

#pragma once

#include <mathlib/mathlib.h>
#include <matrix/math.hpp>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/distance_sensor.h>


/*
* This class can be used to estimate distance to the ground using a laser range finder.
* It's assumed that the laser points down vertically if the vehicle is in it's neutral pose.
* The predict(...) function will do a state prediciton based on accelerometer inputs. It also
* considers accelerometer bias.
* The measurement_update(...) function does a measurement update based on range finder and gps
* velocity measurements. Both functions should always be called together when there is new
* acceleration data available.
* The is_valid() function provides information whether the estimate is valid.
*/

class __EXPORT TerrainEstimator
{
public:
	TerrainEstimator();
	~TerrainEstimator() = default;

	bool is_valid() {return _terrain_valid;}
	float get_distance_to_ground() {return -_x(0);}
	float get_velocity() {return _x(1);}

	void predict(float dt, const struct vehicle_attitude_s *attitude, const struct sensor_combined_s *sensor,
		     const struct distance_sensor_s *distance);
	void measurement_update(uint64_t time_ref, const struct sensor_gps_s *gps,
				const struct distance_sensor_s *distance,
				const struct vehicle_attitude_s *attitude);

private:
	enum {n_x = 3};

	float _distance_last;
	bool _terrain_valid;

	// kalman filter variables
	matrix::Vector<float, n_x> _x;		// state: ground distance, velocity, accel bias in z direction
	float  _u_z;			// acceleration in earth z direction
	matrix::Matrix<float, 3, 3> _P;	// covariance matrix

	// timestamps
	uint64_t _time_last_distance;
	uint64_t _time_last_gps;

	/*
	struct {
		float var_acc_z;
		float var_p_z;
		float var_p_vz;
		float var_lidar;
		float var_gps_vz;
	} _params;
	*/

	bool is_distance_valid(float distance);

};
