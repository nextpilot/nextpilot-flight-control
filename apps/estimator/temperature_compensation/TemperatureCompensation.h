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
 * @file temperature_compensation.h
 *
 * Sensor correction methods
 *
 * @author Paul Riseborough <gncsolns@gmail.com>
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#pragma once

#include <param/param.h>
#include <mathlib/mathlib.h>
#include <matrix/math.hpp>

namespace temperature_compensation {

static constexpr uint8_t GYRO_COUNT_MAX  = 4;
static constexpr uint8_t ACCEL_COUNT_MAX = 4;
static constexpr uint8_t BARO_COUNT_MAX  = 4;

static_assert(GYRO_COUNT_MAX == 4, "GYRO_COUNT_MAX must be 4 (if changed, add/remove TC_* params to match the count)");
static_assert(ACCEL_COUNT_MAX == 4,
              "ACCEL_COUNT_MAX must be 4 (if changed, add/remove TC_* params to match the count)");
static_assert(BARO_COUNT_MAX == 4, "BARO_COUNT_MAX must be 4 (if changed, add/remove TC_* params to match the count)");

static constexpr uint8_t SENSOR_COUNT_MAX = 4;

/**
 ** class TemperatureCompensation
 * Applies temperature compensation to sensor data. Loads the parameters from PX4 param storage.
 */
class TemperatureCompensation {
public:
    /** (re)load the parameters. Make sure to call this on startup as well */
    int parameters_update();

    /** supply information which device_id matches a specific uORB topic_instance
     *  (needed if a system has multiple sensors of the same type)
     *  @return index for compensation parameter entry containing matching device ID on success, <0 otherwise */
    int set_sensor_id_gyro(uint32_t device_id, int topic_instance);
    int set_sensor_id_accel(uint32_t device_id, int topic_instance);
    int set_sensor_id_baro(uint32_t device_id, int topic_instance);

    /**
     * Apply Thermal corrections to gyro (& other) sensor data.
     * @param topic_instance uORB topic instance
     * @param sensor_data input sensor data, output sensor data with applied corrections
     * @param temperature measured current temperature
     * @param offsets returns offsets that were applied (length = 3, except for baro), depending on return value
     * @return -1: error: correction enabled, but no sensor mapping set (@see set_sendor_id_gyro)
     *         0: no changes (correction not enabled),
     *         1: corrections applied but no changes to offsets,
     *         2: corrections applied and offsets updated
     */
    int update_offsets_gyro(int topic_instance, float temperature, float *offsets);
    int update_offsets_accel(int topic_instance, float temperature, float *offsets);
    int update_offsets_baro(int topic_instance, float temperature, float *offsets);

    /** output current configuration status to console */
    void print_status();

private:
    /* Struct containing parameters used by the single axis 5th order temperature compensation algorithm

    Input:

    measured_temp : temperature measured at the sensor (deg C)
    raw_value : reading from the sensor before compensation
    corrected_value : reading from the sensor after compensation for errors

    Compute:

    delta_temp = measured_temp - ref_temp
    offset = x5 * delta_temp^5 + x4 * delta_temp^4 + x3 * delta_temp^3 + x2 * delta_temp^2 + x1 * delta_temp + x0
    corrected_value = raw_value - offset

    */
    struct SensorCalData1D {
        int32_t ID;
        float   x5;
        float   x4;
        float   x3;
        float   x2;
        float   x1;
        float   x0;
        float   ref_temp;
        float   min_temp;
        float   max_temp;
    };

    struct SensorCalHandles1D {
        param_t ID;
        param_t x5;
        param_t x4;
        param_t x3;
        param_t x2;
        param_t x1;
        param_t x0;
        param_t ref_temp;
        param_t min_temp;
        param_t max_temp;
    };

    /* Struct containing parameters used by the 3-axis 3rd order temperature compensation algorithm

    Input:

    measured_temp : temperature measured at the sensor (deg C)
    raw_value[3] : XYZ readings from the sensor before compensation
    corrected_value[3] : XYZ readings from the sensor after compensation for errors

    Compute for each measurement index:

    delta_temp = measured_temp - ref_temp
    offset = x3 * delta_temp^3 + x2 * delta_temp^2 + x1 * delta_temp + x0
    corrected_value = raw_value - offset

     */
    struct SensorCalData3D {
        int32_t ID;       /**< sensor device ID*/
        float   x3[3];    /**< x^3 term of polynomial */
        float   x2[3];    /**< x^2 term of polynomial */
        float   x1[3];    /**< x^1 term of polynomial */
        float   x0[3];    /**< x^0 / offset term of polynomial */
        float   ref_temp; /**< reference temperature used by the curve-fit */
        float   min_temp; /**< minimum temperature with valid compensation data */
        float   max_temp; /**< maximum temperature with valid compensation data */
    };

    struct SensorCalHandles3D {
        param_t ID;
        param_t x3[3];
        param_t x2[3];
        param_t x1[3];
        param_t x0[3];
        param_t ref_temp;
        param_t min_temp;
        param_t max_temp;
    };

    // create a struct containing all thermal calibration parameters
    struct Parameters {
        int32_t         gyro_tc_enable{0};
        SensorCalData3D gyro_cal_data[GYRO_COUNT_MAX]{};

        int32_t         accel_tc_enable{0};
        SensorCalData3D accel_cal_data[ACCEL_COUNT_MAX]{};

        int32_t         baro_tc_enable{0};
        SensorCalData1D baro_cal_data[BARO_COUNT_MAX]{};
    };

    // create a struct containing the handles required to access all calibration parameters
    struct ParameterHandles {
        param_t            gyro_tc_enable{PARAM_INVALID};
        SensorCalHandles3D gyro_cal_handles[GYRO_COUNT_MAX]{};

        param_t            accel_tc_enable{PARAM_INVALID};
        SensorCalHandles3D accel_cal_handles[ACCEL_COUNT_MAX]{};

        param_t            baro_tc_enable{PARAM_INVALID};
        SensorCalHandles1D baro_cal_handles[BARO_COUNT_MAX]{};
    };

    /**
     * initialize ParameterHandles struct
     * @return 0 on succes, <0 on error
     */
    static int initialize_parameter_handles(ParameterHandles &parameter_handles);

    /**

    Calculate the offset required to compensate the sensor for temperature effects using a 5th order method
    If the measured temperature is outside the calibration range, clip the temperature to remain within the range and return false.
    If the measured temperature is within the calibration range, return true.

    Arguments:

    coef : reference to struct containing calibration coefficients
    measured_temp : temperature measured at the sensor (deg C)
    offset : reference to sensor offset

    Returns:

    Boolean true if the measured temperature is inside the valid range for the compensation

    */
    bool calc_thermal_offsets_1D(SensorCalData1D &coef, float measured_temp, float &offset);

    /**

    Calculate the offsets required to compensate the sensor for temperature effects
    If the measured temperature is outside the calibration range, clip the temperature to remain within the range and return false.
    If the measured temperature is within the calibration range, return true.

    Arguments:

    coef : reference to struct containing calibration coefficients
    measured_temp : temperature measured at the sensor (deg C)
    offset : reference to sensor offset - array of 3

    Returns:

    Boolean true if the measured temperature is inside the valid range for the compensation

    */
    bool calc_thermal_offsets_3D(const SensorCalData3D &coef, float measured_temp, float offset[]);

    Parameters _parameters;

    struct PerSensorData {
        PerSensorData() {
            for (int i = 0; i < SENSOR_COUNT_MAX; ++i) {
                device_mapping[i]   = 255;
                last_temperature[i] = -100.0f;
            }
        }

        void reset_temperature() {
            for (int i = 0; i < SENSOR_COUNT_MAX; ++i) {
                last_temperature[i] = -100.0f;
            }
        }

        uint8_t device_mapping[SENSOR_COUNT_MAX]{}; /// map a topic instance to the parameters index
        float   last_temperature[SENSOR_COUNT_MAX]{};
    };

    PerSensorData _gyro_data;
    PerSensorData _accel_data;
    PerSensorData _baro_data;

    template <typename T>
    static inline int set_sensor_id(uint32_t device_id, int topic_instance, PerSensorData &sensor_data,
                                    const T *sensor_cal_data, uint8_t sensor_count_max);
};

} // namespace temperature_compensation
