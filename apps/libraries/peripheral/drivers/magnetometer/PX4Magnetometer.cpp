/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include "PX4Magnetometer.hpp"

#include <drivers/device/Device.hpp>

PX4Magnetometer::PX4Magnetometer(uint32_t device_id, enum Rotation rotation) :
	_device_id{device_id},
	_rotation{rotation}
{
}

PX4Magnetometer::~PX4Magnetometer()
{
	_sensor_pub.unadvertise();
}

void PX4Magnetometer::set_device_type(uint8_t devtype)
{
	// current DeviceStructure
	union device::Device::DeviceId device_id;
	device_id.devid = _device_id;

	// update to new device type
	device_id.devid_s.devtype = devtype;

	// copy back
	_device_id = device_id.devid;
}

void PX4Magnetometer::update(const hrt_abstime &timestamp_sample, float x, float y, float z)
{
	sensor_mag_s report;
	report.timestamp_sample = timestamp_sample;
	report.device_id = _device_id;
	report.temperature = _temperature;
	report.error_count = _error_count;

	// Apply rotation (before scaling)
	rotate_3f(_rotation, x, y, z);

	report.x = x * _scale;
	report.y = y * _scale;
	report.z = z * _scale;

	report.timestamp = hrt_absolute_time();
	_sensor_pub.publish(report);
}
