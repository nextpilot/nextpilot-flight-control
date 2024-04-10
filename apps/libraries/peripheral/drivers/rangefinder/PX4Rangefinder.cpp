/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "PX4Rangefinder.hpp"

#include <drivers/device/Device.hpp>

PX4Rangefinder::PX4Rangefinder(const uint32_t device_id, const uint8_t device_orientation)
{
	set_device_id(device_id);
	set_orientation(device_orientation);
	set_rangefinder_type(distance_sensor_s::MAV_DISTANCE_SENSOR_LASER); // Default to type LASER
}

PX4Rangefinder::~PX4Rangefinder()
{
	_distance_sensor_pub.unadvertise();
}

void PX4Rangefinder::set_device_type(uint8_t device_type)
{
	// current DeviceStructure
	union device::Device::DeviceId device_id;
	device_id.devid = _distance_sensor_pub.get().device_id;

	// update to new device type
	device_id.devid_s.devtype = device_type;

	// copy back to report
	_distance_sensor_pub.get().device_id = device_id.devid;
}

void PX4Rangefinder::set_orientation(const uint8_t device_orientation)
{
	_distance_sensor_pub.get().orientation = device_orientation;
}

void PX4Rangefinder::update(const hrt_abstime &timestamp_sample, const float distance, const int8_t quality)
{
	distance_sensor_s &report = _distance_sensor_pub.get();

	report.timestamp = timestamp_sample;
	report.current_distance = distance;
	report.signal_quality = quality;

	// if quality is unavailable (-1) set to 0 if distance is outside bounds
	if (quality < 0) {
		if ((distance < report.min_distance) || (distance > report.max_distance)) {
			report.signal_quality = 0;
		}
	}

	_distance_sensor_pub.update();
}
