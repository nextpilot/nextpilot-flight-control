/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_H__
#define __PARAM_H__

#include <rtthread.h>
#include <rtdbg.h>
#include <stdlib.h>
#include "param_interface.h"
#include "param_storage.h"

#ifdef __cplusplus
static inline int get_parameter_value(const char *option, int &value) {
    value = 0;

    /* check if this is a param name */
    if (rt_strncmp("p:", option, 2) == 0) {
        const char *param_name = option + 2;

        /* user wants to use a param name */
        param_t param_handle = param_find(param_name);

        if (param_handle != PARAM_INVALID) {
            if (param_get_type(param_handle) != PARAM_TYPE_INT32) {
                return -1;
            }

            int32_t pwm_parm;
            int     ret = param_get(param_handle, &pwm_parm);

            if (ret != 0) {
                return ret;
            }

            value = pwm_parm;

        } else {
            LOG_E("param '%s' not found", param_name);
            return -1;
        }

    } else {
        char *ep;
        value = strtol(option, &ep, 0);

        if (*ep != '\0') {
            return -1;
        }
    }

    return 0;
}
#endif // __cplusplus

#endif //__PARAM_H__
