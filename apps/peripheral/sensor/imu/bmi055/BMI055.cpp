/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include "BMI055.hpp"

#include "BMI055_Accelerometer.hpp"
#include "BMI055_Gyroscope.hpp"

I2CSPIDriverBase *BMI055::instantiate(const I2CSPIDriverConfig &config, int runtime_instance) {
    BMI055 *instance = nullptr;

    if (config.devid_driver_index == DRV_ACC_DEVTYPE_BMI055) {
        instance = new Bosch::BMI055::Accelerometer::BMI055_Accelerometer(config);

    } else if (config.devid_driver_index == DRV_GYR_DEVTYPE_BMI055) {
        instance = new Bosch::BMI055::Gyroscope::BMI055_Gyroscope(config);
    }

    if (!instance) {
        LOG_E("alloc failed");
        return nullptr;
    }

    if (OK != instance->init()) {
        delete instance;
        return nullptr;
    }

    return instance;
}

BMI055::BMI055(const I2CSPIDriverConfig &config) :
    SPI(config),
    I2CSPIDriver(config),
    _drdy_gpio(config.drdy_gpio) {
}

int BMI055::init() {
    int ret = SPI::init();

    if (ret != RT_EOK) {
        DEVICE_DEBUG("SPI::init failed (%i)", ret);
        return ret;
    }

    return Reset() ? 0 : -1;
}

bool BMI055::Reset() {
    _state = STATE::RESET;
    ScheduleClear();
    ScheduleNow();
    return true;
}
