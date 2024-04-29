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

#include "common.h"
#include "polyfit.hpp"

class TemperatureCalibrationAccel : public TemperatureCalibrationCommon<3, 3> {
public:
    TemperatureCalibrationAccel(float min_temperature_rise, float min_start_temperature, float max_start_temperature);
    virtual ~TemperatureCalibrationAccel();

    /**
     * @see TemperatureCalibrationBase::finish()
     */
    int finish();

private:
    virtual inline int update_sensor_instance(PerSensorData &data, orb_subscr_t sensor_sub);

    inline int finish_sensor_instance(PerSensorData &data, int sensor_index);
};
