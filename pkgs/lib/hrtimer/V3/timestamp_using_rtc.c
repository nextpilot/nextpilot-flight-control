/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "hrtimer.h"
#include <rtdevice.h>

// 建议使用HAL直接操作RTC，STM32H7支持亚秒，但是部分MCU不支持亚秒

hrt_abstime hrt_absolute_time(void) {
    static rt_device_t device = RT_NULL;

    if (!device) {
        device = rt_device_find("rtc");
    }

    if (device) {
        if (rt_device_open(device, 0) == RT_EOK) {
            struct timeval tv  = {0};
            rt_err_t       ret = rt_device_control(device, RT_DEVICE_CTRL_RTC_GET_TIMEVAL, &tv);
            rt_device_close(device);
            return 1000000ULL * tv.tv_sec + tv.tv_usec;
        }
    }

    return 0;
}
