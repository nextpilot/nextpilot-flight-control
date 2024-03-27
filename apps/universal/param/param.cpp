/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define DBG_ENABLE
#define DBG_LEVEL DBG_LOG

#include <rtthread.h>
#include <rtdbg.h>
#include "param.h"
#include "param_interface.h"

void param_notify_changes() {
#ifdef SYS_USING_UORB
    static uint32_t           param_instance = 0;
    struct parameter_update_s pup;
    pup.instance  = param_instance++;
    pup.timestamp = hrt_absolute_time();
    orb_publish(ORB_ID(parameter_update), NULL, &pup);
#endif // SYS_USING_UORB
}

// int param_find(const char *name) {
//     return param_find_internal(name, true);
// }

void param_print(param_t idx, bool show_header) {
    if (!param_in_range(idx)) {
        return;
    }

    if (show_header) {
        LOG_RAW("  flag: BIT2=disarm,  BIT1=restart, BIT0=system\n");
        LOG_RAW("status: BIT2=unsaved, BIT1=changed, BIT0=actived\n");
        LOG_RAW("index    type         name            default       current     flag   status\n");
        LOG_RAW("-----  --------  ----------------  ------------  ------------  ------  ------\n");
    }

    param_value_t def_val = param_get_default_value(idx);

    param_value_t  now_val;
    param_status_t status;
    param_get(idx, &now_val);
    param_get_status(idx, &status);

    switch (param_get_type(idx)) {
    case PARAM_TYPE_INT32:
        LOG_RAW("%5d  %8s  %16s  %12d  %12d  0x%04x  0x%04x\n", idx,
                param_get_type_cstr(idx),
                param_get_name(idx),
                def_val.i32,
                now_val.i32,
                param_get_flag(idx).value,
                status.value);
        break;
    case PARAM_TYPE_FLOAT:
        LOG_RAW("%5d  %8s  %16s  %12d  %12d  0x%04x  0x%04x\n", idx,
                param_get_type_cstr(idx),
                param_get_name(idx),
                def_val.f32,
                now_val.f32,
                param_get_flag(idx).value,
                status.value);
        break;
    default:
        break;
    }
}

int param_test() {
    const char *name = "BB";

    param_t idx = param_find(name);
    rt_kprintf("\n==============\n name = %s,  idx = %d\n", name, idx);

    param_print(2, true);
    return 0;
}
INIT_APP_EXPORT(param_test);
