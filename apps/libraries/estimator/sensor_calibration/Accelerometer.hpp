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

#include <conversion/rotation.h>
#include <matrix/matrix/math.hpp>
//#include <px4_platform_common/px4_config.h>
#include <ulog/log.h>
#include <uORB/uORBSubscription.hpp>
#include <uORB/topics/sensor_correction.h>

using namespace nextpilot;

namespace calibration
{
class Accelerometer
{
public:
	static constexpr int MAX_SENSOR_COUNT = 4;

	static constexpr uint8_t DEFAULT_PRIORITY = 50;
	static constexpr uint8_t DEFAULT_EXTERNAL_PRIORITY = 75;

	static constexpr const char *SensorString() { return "ACC"; }

	Accelerometer();
	explicit Accelerometer(uint32_t device_id);

	~Accelerometer() = default;

	void PrintStatus();

	bool set_calibration_index(int calibration_index);
	void set_device_id(uint32_t device_id);
	bool set_offset(const matrix::Vector3f &offset);
	bool set_scale(const matrix::Vector3f &scale);
	void set_rotation(Rotation rotation);

	bool calibrated() const { return (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_count() const { return _calibration_count; }
	int8_t calibration_index() const { return _calibration_index; }
	uint32_t device_id() const { return _device_id; }
	bool enabled() const { return (_priority > 0); }
	bool external() const { return _external; }
	const matrix::Vector3f &offset() const { return _offset; }
	const int32_t &priority() const { return _priority; }
	const matrix::Dcmf &rotation() const { return _rotation; }
	const Rotation &rotation_enum() const { return _rotation_enum; }
	const matrix::Vector3f &scale() const { return _scale; }

	// apply offsets and scale
	// rotate corrected measurements from sensor to body frame
	inline matrix::Vector3f Correct(const matrix::Vector3f &data) const
	{
		return _rotation * matrix::Vector3f{(data - _thermal_offset - _offset).emult(_scale)};
	}

	// Compute sensor offset from bias (board frame)
	matrix::Vector3f BiasCorrectedSensorOffset(const matrix::Vector3f &bias) const
	{
		// updated calibration offset = existing offset + bias rotated to sensor frame and unscaled
		return _offset + (_rotation.I() * bias).edivide(_scale);
	}

	bool ParametersLoad();
	bool ParametersSave(int desired_calibration_index = -1, bool force = false);
	void ParametersUpdate();

	void Reset();

	void SensorCorrectionsUpdate(bool force = false);

private:
	uORB::Subscription _sensor_correction_sub{ORB_ID(sensor_correction)};

	Rotation _rotation_enum{ROTATION_NONE};

	matrix::Dcmf _rotation;
	matrix::Vector3f _offset;
	matrix::Vector3f _scale;
	matrix::Vector3f _thermal_offset;

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	int32_t _priority{-1};

	bool _external{false};

	uint8_t _calibration_count{0};
};
} // namespace calibration
