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

//#include <px4_platform_common/px4_config.h>
#include <rtdbg.h>
#include <conversion/rotation.h>
#include <mathlib/mathlib.h>
#include <param/param.h>
#include <matrix/math.hpp>

namespace calibration
{

/**
 * @brief Find sensor's calibration index if it exists.
 *
 * @param sensor_type Calibration parameter abbreviated sensor string ("ACC", "GYRO", "MAG")
 * @param device_id
 * @return int8_t Valid calibration index on success, -1 otherwise
 */
int8_t FindCurrentCalibrationIndex(const char *sensor_type, uint32_t device_id);

/**
 * @brief Find sensor's calibration index if it exists, otherwise select an available slot.
 *
 * @param sensor_type Calibration parameter abbreviated sensor string ("ACC", "GYRO", "MAG")
 * @param device_id
 * @param preferred_index preferred index (optional)
 * @return int8_t Valid calibration index on success, -1 otherwise
 */
int8_t FindAvailableCalibrationIndex(const char *sensor_type, uint32_t device_id, int8_t preferred_index = -1);

/**
 * @brief Get sensor calibration parameter value.
 *
 * @param sensor_type Calibration parameter abbreviated sensor string ("ACC", "GYRO", "MAG")
 * @param cal_type Calibration parameter abbreviated type ("OFF", "SCALE", "ROT", "PRIO")
 * @param instance
 * @return int32_t The calibration value.
 */
int32_t GetCalibrationParamInt32(const char *sensor_type, const char *cal_type, uint8_t instance);
float GetCalibrationParamFloat(const char *sensor_type, const char *cal_type, uint8_t instance);

/**
 * @brief Set a single calibration paramter.
 *
 * @param sensor_type Calibration parameter abbreviated sensor string ("ACC", "GYRO", "MAG")
 * @param cal_type Calibration parameter abbreviated type ("OFF", "SCALE", "ROT", "PRIO")
 * @param instance Calibration index (0 - 3)
 * @param value int32_t parameter value
 * @return true if the parameter name was valid and value saved successfully, false otherwise.
 */
template<typename T>
bool SetCalibrationParam(const char *sensor_type, const char *cal_type, uint8_t instance, T value)
{
	char str[20] {};

	// eg CAL_MAGn_ID/CAL_MAGn_ROT
	sprintf(str, "CAL_%s%u_%s", sensor_type, instance, cal_type);

	int ret = param_set_no_notification(param_find(str), &value);

	if (ret != PX4_OK) {
		PX4_ERR("failed to set %s", str);
	}

	return ret == PX4_OK;
}

/**
 * @brief Get the Calibration Params Vector 3f object
 *
 * @param sensor_type Calibration parameter abbreviated sensor string ("ACC", "GYRO", "MAG")
 * @param cal_type Calibration parameter abbreviated type ("OFF", "SCALE", "ROT", "PRIO")
 * @param instance Calibration index (0 - 3)
 * @return matrix::Vector3f Vector of calibration values.
 */
matrix::Vector3f GetCalibrationParamsVector3f(const char *sensor_type, const char *cal_type, uint8_t instance);

/**
 * @brief Set the Calibration Params Vector 3f object
 *
 * @param sensor_type Calibration parameter abbreviated sensor string ("ACC", "GYRO", "MAG")
 * @param cal_type Calibration parameter abbreviated type ("OFF", "SCALE", "ROT", "PRIO")
 * @param instance Calibration index (0 - 3)
 * @param values Vector of calibration values x, y, z.
 * @return true if the parameter name was valid and all values saved successfully, false otherwise.
 */
bool SetCalibrationParamsVector3f(const char *sensor_type, const char *cal_type, uint8_t instance,
				  matrix::Vector3f values);

/**
 * @brief Get the board sensor level adjustment (SENS_BOARD_X_OFF, SENS_BOARD_Y_OFF, SENS_BOARD_Z_OFF).
 *
 * @return matrix::Eulerf
 */
matrix::Eulerf GetSensorLevelAdjustment();

/**
 * @brief Get the board rotation.
 *
 * @return enum Rotation
 */
Rotation GetBoardRotation();

/**
 * @brief Get the board rotation Dcm.
 *
 * @return matrix::Dcmf
 */
matrix::Dcmf GetBoardRotationMatrix();

/**
 * @brief Determine if device is on an external bus
 *
 * @return true if device is on an external bus
 */
bool DeviceExternal(uint32_t device_id);

} // namespace calibration
