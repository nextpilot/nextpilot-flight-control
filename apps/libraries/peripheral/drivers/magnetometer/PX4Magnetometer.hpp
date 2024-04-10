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
#include <uORB/topics/sensor_mag.h>

class PX4Magnetometer
{
public:
	PX4Magnetometer(uint32_t device_id, enum Rotation rotation = ROTATION_NONE);
	~PX4Magnetometer();

	void set_device_id(uint32_t device_id) { _device_id = device_id; }
	void set_device_type(uint8_t devtype);
	void set_error_count(uint32_t error_count) { _error_count = error_count; }
	void set_scale(float scale) { _scale = scale; }
	void set_temperature(float temperature) { _temperature = temperature; }

	void update(const hrt_abstime &timestamp_sample, float x, float y, float z);

	int get_instance() { return _sensor_pub.get_instance(); };

private:
	uORB::PublicationMulti<sensor_mag_s> _sensor_pub{ORB_ID(sensor_mag)};

	uint32_t		_device_id{0};
	const enum Rotation	_rotation;

	float			_scale{1.f};
	float			_temperature{NAN};
	uint32_t		_error_count{0};
};
