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
#include <lib/conversion/rotation.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/sensor_gyro_fifo.h>

class PX4Gyroscope
{
public:
	PX4Gyroscope(uint32_t device_id, enum Rotation rotation = ROTATION_NONE);
	~PX4Gyroscope();

	uint32_t get_device_id() const { return _device_id; }

	int32_t get_max_rate_hz() const { return math::constrain(_imu_gyro_rate_max, static_cast<int32_t>(100), static_cast<int32_t>(4000)); }

	void set_device_id(uint32_t device_id) { _device_id = device_id; }
	void set_device_type(uint8_t devtype);
	void set_error_count(uint32_t error_count) { _error_count = error_count; }
	void set_range(float range) { _range = range; UpdateClipLimit(); }
	void set_scale(float scale);
	void set_temperature(float temperature) { _temperature = temperature; }

	void update(const hrt_abstime &timestamp_sample, float x, float y, float z);

	void updateFIFO(sensor_gyro_fifo_s &sample);

	int get_instance() { return _sensor_pub.get_instance(); };

private:
	void UpdateClipLimit();

	uORB::PublicationMulti<sensor_gyro_s> _sensor_pub{ORB_ID(sensor_gyro)};
	uORB::PublicationMulti<sensor_gyro_fifo_s>  _sensor_fifo_pub{ORB_ID(sensor_gyro_fifo)};

	uint32_t		_device_id{0};
	const enum Rotation	_rotation;

	int32_t			_imu_gyro_rate_max{0};

	float			_range{math::radians(2000.f)};
	float			_scale{1.f};
	float			_temperature{NAN};

	float			_clip_limit{_range / _scale};

	uint32_t		_error_count{0};

	int16_t			_last_sample[3] {};
};
