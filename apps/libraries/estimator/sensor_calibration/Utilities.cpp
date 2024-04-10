/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

//#include <px4_platform_common/px4_config.h>
#include <rtdbg.h>
#include <conversion/rotation.h>

#include <drivers/device/Device.hpp>
#include <mathlib/mathlib.h>
#include <param/param.h>

#if defined(CONFIG_I2C)
# include <px4_platform_common/i2c.h>
#endif // CONFIG_I2C

#if defined(CONFIG_SPI)
# include <px4_platform_common/spi.h>
#endif // CONFIG_SPI

using math::radians;
using matrix::Eulerf;
using matrix::Dcmf;
using matrix::Vector3f;

namespace calibration
{

static constexpr int MAX_SENSOR_COUNT = 4; // TODO: per sensor?

int8_t FindCurrentCalibrationIndex(const char *sensor_type, uint32_t device_id)
{
	if (device_id == 0) {
		return -1;
	}

	for (unsigned i = 0; i < MAX_SENSOR_COUNT; ++i) {
		char str[20] {};
		sprintf(str, "CAL_%s%u_ID", sensor_type, i);

		int32_t device_id_val = 0;

		param_t param_handle = param_find_no_notification(str);

		if (param_handle == PARAM_INVALID) {
			continue;
		}

		// find again and get value, but this time mark it active
		if (param_get(param_find(str), &device_id_val) != OK) {
			continue;
		}

		if ((uint32_t)device_id_val == device_id) {
			return i;
		}
	}

	return -1;
}

int8_t FindAvailableCalibrationIndex(const char *sensor_type, uint32_t device_id, int8_t preferred_index)
{
	// if this device is already using a calibration slot then keep it
	int calibration_index = FindCurrentCalibrationIndex(sensor_type, device_id);

	if (calibration_index >= 0) {
		return calibration_index;
	}


	// device isn't currently using a calibration slot, select user preference (preferred_index)
	//  if available, otherwise use the first available slot
	uint32_t cal_device_ids[MAX_SENSOR_COUNT] {};

	for (unsigned i = 0; i < MAX_SENSOR_COUNT; ++i) {
		char str[20] {};
		sprintf(str, "CAL_%s%u_ID", sensor_type, i);
		int32_t device_id_val = 0;

		if (param_get(param_find_no_notification(str), &device_id_val) == PX4_OK) {
			cal_device_ids[i] = device_id_val;
		}
	}

	// use preferred_index if it's available
	if ((preferred_index >= 0) && (preferred_index < MAX_SENSOR_COUNT)
	    && (cal_device_ids[preferred_index] == 0)) {

		calibration_index = preferred_index;

	} else {
		// otherwise select first available slot
		for (int i = 0; i < MAX_SENSOR_COUNT; i++) {
			if (cal_device_ids[i] == 0) {
				calibration_index = i;
				break;
			}
		}
	}

	if (calibration_index == -1) {
		PX4_ERR("no %s calibration slots available", sensor_type);
	}

	return calibration_index;
}

int32_t GetCalibrationParamInt32(const char *sensor_type, const char *cal_type, uint8_t instance)
{
	// eg CAL_MAGn_ID/CAL_MAGn_ROT
	char str[20] {};
	sprintf(str, "CAL_%s%" PRIu8 "_%s", sensor_type, instance, cal_type);

	int32_t value = 0;

	if (param_get(param_find(str), &value) != 0) {
		PX4_ERR("failed to get %s", str);
	}

	return value;
}

float GetCalibrationParamFloat(const char *sensor_type, const char *cal_type, uint8_t instance)
{
	// eg CAL_BAROn_OFF
	char str[20] {};
	sprintf(str, "CAL_%s%" PRIu8 "_%s", sensor_type, instance, cal_type);

	float value = NAN;

	if (param_get(param_find(str), &value) != 0) {
		PX4_ERR("failed to get %s", str);
	}

	return value;
}

Vector3f GetCalibrationParamsVector3f(const char *sensor_type, const char *cal_type, uint8_t instance)
{
	Vector3f values{0.f, 0.f, 0.f};

	char str[20] {};

	for (int axis = 0; axis < 3; axis++) {
		char axis_char = 'X' + axis;

		// eg CAL_MAGn_{X,Y,Z}OFF
		sprintf(str, "CAL_%s%" PRIu8 "_%c%s", sensor_type, instance, axis_char, cal_type);

		if (param_get(param_find(str), &values(axis)) != 0) {
			PX4_ERR("failed to get %s", str);
		}
	}

	return values;
}

bool SetCalibrationParamsVector3f(const char *sensor_type, const char *cal_type, uint8_t instance, Vector3f values)
{
	int ret = PX4_OK;
	char str[20] {};

	for (int axis = 0; axis < 3; axis++) {
		char axis_char = 'X' + axis;

		// eg CAL_MAGn_{X,Y,Z}OFF
		sprintf(str, "CAL_%s%" PRIu8 "_%c%s", sensor_type, instance, axis_char, cal_type);

		if (param_set_no_notification(param_find(str), &values(axis)) != 0) {
			PX4_ERR("failed to set %s = %.4f", str, (double)values(axis));
			ret = PX4_ERROR;
		}
	}

	return ret == PX4_OK;
}

Eulerf GetSensorLevelAdjustment()
{
	float x_offset = 0.f;
	float y_offset = 0.f;
	float z_offset = 0.f;
	param_get(param_find("SENS_BOARD_X_OFF"), &x_offset);
	param_get(param_find("SENS_BOARD_Y_OFF"), &y_offset);
	param_get(param_find("SENS_BOARD_Z_OFF"), &z_offset);

	return Eulerf{radians(x_offset), radians(y_offset), radians(z_offset)};
}

enum Rotation GetBoardRotation()
{
	// get transformation matrix from sensor/board to body frame
	int32_t board_rot = -1;
	param_get(param_find("SENS_BOARD_ROT"), &board_rot);

	if (board_rot >= 0 && board_rot <= Rotation::ROTATION_MAX) {
		return static_cast<enum Rotation>(board_rot);

	} else {
		PX4_ERR("invalid SENS_BOARD_ROT: %" PRId32, board_rot);
	}

	return Rotation::ROTATION_NONE;
}

Dcmf GetBoardRotationMatrix()
{
	return get_rot_matrix(GetBoardRotation());
}

bool DeviceExternal(uint32_t device_id)
{
	bool external = true;

	// decode device id to determine if external
	union device::Device::DeviceId id {};
	id.devid = device_id;

	const device::Device::DeviceBusType bus_type = id.devid_s.bus_type;

	switch (bus_type) {
	case device::Device::DeviceBusType_I2C:
#if defined(CONFIG_I2C)
		external = px4_i2c_device_external(device_id);
#endif // CONFIG_I2C
		break;

	case device::Device::DeviceBusType_SPI:
#if defined(CONFIG_SPI)
		external = px4_spi_bus_external(id.devid_s.bus);
#endif // CONFIG_SPI
		break;

	case device::Device::DeviceBusType_UAVCAN:
		external = true;
		break;

	case device::Device::DeviceBusType_SIMULATION:
		external = false;
		break;

	case device::Device::DeviceBusType_SERIAL:
		external = true;
		break;

	case device::Device::DeviceBusType_MAVLINK:
		external = true;
		break;

	case device::Device::DeviceBusType_UNKNOWN:
		external = true;
		break;
	}

	return external;
}

} // namespace calibration
