/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "pa_gs02b_main"
#define LOG_LVL LOG_LVL_INFO

#include "pa_gs02b.hpp"

static bool      _pa_gs02b_init_flag = false;
static uint8_t   _pa_gs02b_thread_stack[2048];
static rt_thread _pa_gs02b_thread_handle;

void pa_gs02b_thread_entry(void *parameter) {
    _pa_gs02b_init_flag = !pa_gs02b_init();

    while (_pa_gs02b_init_flag) {
        pa_gs02b_run();
        rt_thread_mdelay(ATTITUDE_PERIOD_MS);
    }
}

int pa_gs02b_start() {
    const airframe_config_t *config = airframe_get_config();
    if (config->sys_hitl > 0) {
        return 0;
    }

    rt_err_t ret = rt_thread_init(&_pa_gs02b_thread_handle,
                                  "pa_gs02b",
                                  pa_gs02b_thread_entry,
                                  RT_NULL,
                                  &_pa_gs02b_thread_stack[0],
                                  sizeof(_pa_gs02b_thread_stack),
                                  15,
                                  5);

    if (!ret) {
        if (rt_thread_startup(&_pa_gs02b_thread_handle) == RT_EOK) {
            LOG_I("start ok");
        }
    } else {
        LOG_E("start fail");
        ret = RT_ERROR;
    }

    return ret;
}

INIT_APP_EXPORT(pa_gs02b_start);
