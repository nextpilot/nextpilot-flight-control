/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "ekf2_fake_main"
#define LOG_LVL LOG_LVL_INFO

#include "ekf2_fake.hpp"

Ekf2Fake *_instance{nullptr};
rt_thread _ekf2_fake_thread_handle;
char      _ekf2_fake_thread_stack[2048];

static void ekf2_fake_run(void *parameter) {
    _instance->init();
    while (1) {
        _instance->Run();
        rt_thread_mdelay(EKF2_FAKE_PERIOD_MS);
    }
}

static int ekf2_fake_start(void) {
    int32_t sys_hitl = 0;
    param_get(param_find("SYS_HITL"), &sys_hitl);
    if (sys_hitl != 2) {
        LOG_I("Not in sitl, sys_hitl: %d", sys_hitl);
        return RT_EOK;
    } else {
        _instance       = new Ekf2Fake();
        rt_err_t result = rt_thread_init(&_ekf2_fake_thread_handle,
                                         "ekf2_fake",
                                         ekf2_fake_run,
                                         RT_NULL,
                                         &_ekf2_fake_thread_stack[0],
                                         sizeof(_ekf2_fake_thread_stack),
                                         10,
                                         5);

        if (result != RT_EOK) {
            LOG_E("thread init failed");
            return RT_ERROR;
        }

        result = rt_thread_startup(&_ekf2_fake_thread_handle);

        if (result != RT_EOK) {
            LOG_E("thread startup failed");
            return RT_ERROR;
        } else {
            LOG_I("start ok");
            return RT_EOK;
        }
    }
}

INIT_APP_EXPORT(ekf2_fake_start);
