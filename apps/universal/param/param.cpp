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

param_t param_find_no_notification(const char *name) {
    return param_find_internal(name, false);
}

const char *param_get_name(param_t idx) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        return info.name;
    } else {
        return NULL;
    }
}

int param_get_count_used() {
    int            count = 0;
    param_status_t status;

    for (param_t i = 0; i < param_get_count(); i++) {
        param_get_status(i, &status);
        if (status.actived) {
            count++;
        }
    }

    return count;
}

param_type_t param_get_type(param_t idx) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        return info.type;
    } else {
        return PARAM_TYPE_UNKNOWN;
    }
}

const char *param_get_type_cstr(param_t idx) {
    return param_type_cstr(param_get_type(idx));
}

param_flag_t param_get_flag(param_t idx) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        return info.flag;
    } else {
        param_flag_t flag = {.value = 0};
        return flag;
    }
}

param_value_t param_get_default_value(param_t idx) {
    param_info_t info;
    int          ret = param_get_info(idx, &info);
    if (ret == 0) {
        return info.value;
    } else {
        param_value_t val = {.f32 = 0};
        return val;
    }
}

int param_set(param_t idx, const void *val) {
    return param_set_internal(idx, (const param_value_t *)val, false, true);
}

int param_set_no_notification(param_t idx, const void *val) {
    return param_set_internal(idx, (const param_value_t *)val, false, false);
}

int param_set_external() {
    return 0;
}

int param_get(param_t idx, void *val) {
    return param_get_internal(idx, (param_value_t *)val, true);
}

float param_get_float(param_t idx) {
    param_value_t val;
    int           ret = param_get_internal(idx, &val, true);
    if (ret == 0) {
        return val.f32;
    } else {
        return 0.0f / 0.0f;
    }
}

int32_t param_get_int32(param_t idx) {
    param_value_t val;
    int           ret = param_get_internal(idx, &val, true);
    if (ret == 0) {
        return val.f32;
    } else {
        return INT32_MAX;
    }
}

bool param_value_used(param_t idx) {
    param_status_t status;
    return (param_get_status(idx, &status) == 0) ? status.actived : false;
}

bool param_value_unsaved(param_t idx) {
    param_status_t status;

    return (param_get_status(idx, &status) == 0) ? status.unsaved : false;
}

bool param_value_changed(param_t idx) {
    param_status_t status;

    return (param_get_status(idx, &status) == 0) ? status.changed : false;
}

int param_reset(param_t idx) {
    return param_reset_internal(idx, true);
}

int param_reset_no_notification(param_t idx) {
    return param_reset_internal(idx, false);
}

void param_reset_all() {
    //
    for (param_t i = 0; i < param_get_count(); i++) {
        // 重置过程中不发送param_update消息
        param_reset_internal(i, false);
    }
    // 重置结束之后发送param_update消息
    param_notify_changes();
}

void param_reset_all_notification() {
    for (param_t i = 0; i < param_get_count(); i++) {
        param_reset_internal(i, false);
    }
    param_notify_changes();
}

void param_reset_excludes(const char *excludes[], int num_excludes) {
    bool found = false;
    for (param_t idx = 0; idx < param_get_count(); idx++) {
        const char *name    = param_get_name(idx);
        bool        exclude = false;
        for (int kk = 0; kk < num_excludes; kk++) {
            int len = rt_strlen(excludes[kk]);

            if ((excludes[kk][len - 1] == '*' &&
                 rt_strncmp(name, excludes[kk], len - 1) == 0) ||
                rt_strcmp(name, excludes[kk]) == 0) {
                exclude = true;
                break;
            }
        }

        if (!exclude) {
            // 重置过程中不发布param_update消息
            param_reset_internal(idx, false);
            found = true;
        }
    }

    if (found) {
        param_notify_changes();
    }
}

void param_reset_specific(const char *resets[], int num_resets) {
    bool found = false;
    for (param_t idx = 0; idx < param_get_count(); idx++) {
        const char *name  = param_get_name(idx);
        bool        reset = false;

        for (int kk = 0; kk < num_resets; kk++) {
            int len = rt_strlen(resets[kk]);

            if ((resets[kk][len - 1] == '*' &&
                 rt_strncmp(name, resets[kk], len - 1) == 0) ||
                rt_strcmp(name, resets[kk]) == 0) {
                reset = true;
                break;
            }
        }

        if (reset) {
            param_reset_internal(idx, false);
            found = true;
        }
    }

    if (found) {
        param_notify_changes();
    }
}

// int param_export(const char *devname, param_filter_func filter) {
// }

// int param_import(const char *devname, param_filter_func filter) {
// }

// int param_reset_and_import(const char *devname) {
//     param_reset_all_notification();
//     return param_import(devname, NULL);
// }

int param_load_default() {
    return 0;
}

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
