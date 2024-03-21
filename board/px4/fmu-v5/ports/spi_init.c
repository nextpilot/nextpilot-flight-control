/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "BSP"

#include <rtthread.h>
#include <spi_flash.h>
// #include <spi_flash_sfud.h>
#include <drv_spi.h>
#include <rtdbg.h>

static int rt_bsp_icm20602_init() {
#define ICM20602_CS          GET_PIN(F, 3)
#define ICM20602_CS_PORT     GPIOF
#define ICM20602_CS_PIN      GPIO_PIN_3
#define ICM20602_DEVICE_NAME "icm20602"

    // 设备片选
    rt_pin_mode(ICM20602_CS, PIN_MODE_OUTPUT);
    // 挂载设备
    rt_err_t ret = rt_hw_spi_device_attach("spi1", ICM20602_DEVICE_NAME, ICM20602_CS_PORT, ICM20602_CS_PIN);
    if (ret != RT_EOK) {
        LOG_E("icm20602 attach to spi1 failed");
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_bsp_icm20602_init);

static int rt_bsp_bmi055_init() {
#define BMI055_ACCEL_CS          GET_PIN(G, 10)
#define BMI055_ACCEL_CS_PORT     GPIOG
#define BMI055_ACCEL_CS_PIN      GPIO_PIN_10
#define BMI055_ACCEL_DEVICE_NAME "bmi055_accel"

    // 设备片选
    rt_pin_mode(BMI055_ACCEL_CS, PIN_MODE_OUTPUT);
    // 挂载设备
    rt_err_t ret = rt_hw_spi_device_attach("spi1", BMI055_ACCEL_DEVICE_NAME, BMI055_ACCEL_CS_PORT, BMI055_ACCEL_CS_PIN);
    if (ret != RT_EOK) {
        LOG_E("bmi055_accel attach to spi1 failed");
        return -RT_ERROR;
    }

#define BMI055_GYRO_CS          GET_PIN(F, 4)
#define BMI055_GYRO_CS_PORT     GPIOF
#define BMI055_GYRO_CS_PIN      GPIO_PIN_4
#define BMI055_GYRO_DEVICE_NAME "bmi055_gyro"
    // 设备片选
    rt_pin_mode(BMI055_GYRO_CS, PIN_MODE_OUTPUT);
    // 挂载设备
    ret = rt_hw_spi_device_attach("spi1", BMI055_GYRO_DEVICE_NAME, BMI055_GYRO_CS_PORT, BMI055_GYRO_CS_PIN);
    if (ret != RT_EOK) {
        LOG_E("bmi055_gyro attach to spi1 failed");
        return -RT_ERROR;
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_bsp_bmi055_init);

// static int rt_bsp_max14830_init() {
// #define MAX14830_CS          GET_PIN(I, 6)
// #define MAX14830_CS_PORT     GPIOI
// #define MAX14830_CS_PIN      GPIO_PIN_6
// #define MAX14830_DEVICE_NAME "max14830"
//     // 设备片选
//     rt_pin_mode(MAX14830_CS, PIN_MODE_OUTPUT);
//     // 挂载设备
//     rt_err_t ret = rt_hw_spi_device_attach("spi6", MAX14830_DEVICE_NAME, MAX14830_CS_PORT, MAX14830_CS_PIN);
//     if (ret != RT_EOK) {
//         LOG_E("max14830 attach to spi6 failed");
//     }
//     return RT_EOK;
// }
// INIT_ENV_EXPORT(rt_bsp_max14830_init);

static int rt_bsp_ms5611_init() {
#define MS5611_CS          GET_PIN(F, 10)
#define MS5611_CS_PORT     GPIOF
#define MS5611_CS_PIN      GPIO_PIN_10
#define MS5611_DEVICE_NAME "ms5611"
    // 设备片选
    rt_pin_mode(MS5611_CS, PIN_MODE_OUTPUT);
    // 挂载设备
    rt_err_t ret = rt_hw_spi_device_attach("spi4", MS5611_DEVICE_NAME, MS5611_CS_PORT, MS5611_CS_PIN);
    if (ret != RT_EOK) {
        LOG_E("ms5611 attach to spi4 failed");
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_bsp_ms5611_init);

static int rt_bsp_adis16488_init() {
#define ADIS16488_CS          GET_PIN(I, 10)
#define ADIS16488_CS_PORT     GPIOI
#define ADIS16488_CS_PIN      GPIO_PIN_10
#define ADIS16488_DEVICE_NAME "adis16488" // SPI 设备名称
    // 设备片选
    rt_pin_mode(ADIS16488_CS, PIN_MODE_OUTPUT);
    // 挂载设备
    rt_err_t ret = rt_hw_spi_device_attach("spi5", ADIS16488_DEVICE_NAME, ADIS16488_CS_PORT, ADIS16488_CS_PIN);
    if (ret != RT_EOK) {
        LOG_E("adis16488 attach to spi5 failed");
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_bsp_adis16488_init);

static int rt_bsp_fm25v20_init(void) {
#define FM25V02A_CS          GET_PIN(F, 5)
#define FM25V02A_CS_PORT     GPIOF
#define FM25V02A_CS_PIN      GPIO_PIN_5
#define FM25V02A_DEVICE_NAME "fm25v02" // SPI 设备名称
    //__HAL_RCC_GPIOF_CLK_ENABLE();
    rt_pin_mode(FM25V02A_CS, PIN_MODE_OUTPUT);

    rt_err_t ret = rt_hw_spi_device_attach("spi2", FM25V02A_DEVICE_NAME, FM25V02A_CS_PORT, FM25V02A_CS_PIN);
    if (ret != RT_EOK) {
        LOG_E("fm25v02 attach to spi2 failed");
    }

    return RT_EOK;
}
INIT_ENV_EXPORT(rt_bsp_fm25v20_init);
