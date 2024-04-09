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

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/sensor_correction.h>

namespace calibration
{
class Barometer
{
public:
	static constexpr int MAX_SENSOR_COUNT = 4;

	static constexpr uint8_t DEFAULT_PRIORITY = 50;
	static constexpr uint8_t DEFAULT_EXTERNAL_PRIORITY = 75;

	static constexpr const char *SensorString() { return "BARO"; }

	Barometer();
	explicit Barometer(uint32_t device_id);

	~Barometer() = default;

	void PrintStatus();

	bool set_calibration_index(int calibration_index);
	void set_device_id(uint32_t device_id);
	bool set_offset(const float &offset);

	bool calibrated() const { return (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_count() const { return _calibration_count; }
	int8_t calibration_index() const { return _calibration_index; }
	uint32_t device_id() const { return _device_id; }
	bool enabled() const { return (_priority > 0); }
	bool external() const { return _external; }
	const float &offset() const { return _offset; }
	const int32_t &priority() const { return _priority; }
	const float &thermal_offset() const { return _thermal_offset; }

	// apply offsets
	inline float Correct(const float &data) const
	{
		return data - _thermal_offset - _offset;
	}

	inline float Uncorrect(const float &corrected_data) const
	{
		return corrected_data + _thermal_offset + _offset;
	}

	// Compute sensor offset from bias (board frame)
	float BiasCorrectedSensorOffset(const float &bias) const
	{
		return bias + _thermal_offset + _offset;
	}

	bool ParametersLoad();
	bool ParametersSave(int desired_calibration_index = -1, bool force = false);
	void ParametersUpdate();

	void Reset();

	void SensorCorrectionsUpdate(bool force = false);

private:
	uORB::Subscription _sensor_correction_sub{ORB_ID(sensor_correction)};

	float _offset{0};
	float _thermal_offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	int32_t _priority{-1};

	bool _external{false};

	uint8_t _calibration_count{0};
};
} // namespace calibration
