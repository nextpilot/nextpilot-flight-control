/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "BMI088.hpp"

#include "BMI088_Accelerometer.hpp"
#include "BMI088_Gyroscope.hpp"

BMI088 *BMI088::instantiate(int argc, char *argv[]) {
    BMI088 *instance = nullptr;

    if (devtype == DRV_ACC_DEVTYPE_BMI088) {
        instance = new Bosch::BMI088::Accelerometer::BMI088_Accelerometer(config);

    } else if (devtype == DRV_GYR_DEVTYPE_BMI088) {
        instance = new Bosch::BMI088::Gyroscope::BMI088_Gyroscope(config);
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

BMI088::BMI088(const I2CSPIDriverConfig &config) :
    SPI(config),
    I2CSPIDriver(config),
    _drdy_gpio(config.drdy_gpio) {
}

int BMI088::init() {
    int ret = SPI::init();

    if (ret != RT_EOK) {
        DEVICE_DEBUG("SPI::init failed (%i)", ret);
        return ret;
    }

    return Reset() ? 0 : -1;
}

bool BMI088::Reset() {
    _state = STATE::RESET;
    ScheduleClear();
    ScheduleNow();
    return true;
}
