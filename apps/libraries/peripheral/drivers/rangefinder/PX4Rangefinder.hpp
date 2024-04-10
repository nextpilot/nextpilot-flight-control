/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <hrtimer.h>
#include <conversion/rotation.h>

#include <uORB/uORBPublication.hpp>
#include <uORB/topics/distance_sensor.h>

class PX4Rangefinder
{
public:
	PX4Rangefinder(const uint32_t device_id,
		       const uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);
	~PX4Rangefinder();

	// Set the MAV_DISTANCE_SENSOR type (LASER, ULTRASOUND, INFRARED, RADAR)
	void set_rangefinder_type(uint8_t rangefinder_type) { _distance_sensor_pub.get().type = rangefinder_type; };

	void set_device_id(const uint32_t device_id) { _distance_sensor_pub.get().device_id = device_id; };
	void set_device_type(const uint8_t device_type);

	void set_fov(const float fov) { set_hfov(fov); set_vfov(fov); }
	void set_hfov(const float fov) { _distance_sensor_pub.get().h_fov = fov; }
	void set_vfov(const float fov) { _distance_sensor_pub.get().v_fov = fov; }

	void set_max_distance(const float distance) { _distance_sensor_pub.get().max_distance = distance; }
	void set_min_distance(const float distance) { _distance_sensor_pub.get().min_distance = distance; }

	void set_orientation(const uint8_t device_orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING);

	void update(const hrt_abstime &timestamp_sample, const float distance, const int8_t quality = -1);

	int get_instance() { return _distance_sensor_pub.get_instance(); };

private:
	uORB::PublicationMultiData<distance_sensor_s> _distance_sensor_pub{ORB_ID(distance_sensor)};
};
