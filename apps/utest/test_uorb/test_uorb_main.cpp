/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "uorb_test_main"
#define LOG_LVL LOG_LVL_INFO

#include "nextpilot.h"
#include "uorb_pub.hpp"
#include "uorb_sub.hpp"

char      _ut_pub_stack[1024], _ut_sub_stack[1024];
rt_thread _ut_pub_handle, _ut_sub_handle;

void ut_pub_entry(void *param) {
    while (1) {
        uorb_pub::publish_attitude();
        uorb_pub::publish_sensor_accel();
        uorb_pub::publish_sensor_gyro();
        rt_thread_mdelay(1000);
    }
}

void ut_sub_entry(void *param) {
    while (1) {
        uorb_sub::subscribe_attitude();
        uorb_sub::copy_sensor_accel();
        uorb_sub::subscription_interval_sensor_gyro();
        rt_thread_mdelay(1000);
    }
}

int uorb_test_start() {
    rt_err_t ret = RT_EOK;
    ret          = rt_thread_init(&_ut_pub_handle, "uorb_pub", ut_pub_entry, nullptr, &_ut_pub_stack[0], sizeof(_ut_pub_stack), 10, 5);
    ret          = rt_thread_init(&_ut_sub_handle, "uorb_sub", ut_sub_entry, nullptr, &_ut_sub_stack[0], sizeof(_ut_sub_stack), 10, 5);
    rt_thread_startup(&_ut_pub_handle);
    rt_thread_startup(&_ut_sub_handle);
    LOG_I("uorb test start");
    return ret;
}

INIT_APP_EXPORT(uorb_test_start);
