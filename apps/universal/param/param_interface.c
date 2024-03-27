/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "param.interface"

#include <rtthread.h>
#include <rtdbg.h>
#include "param_interface.h"

#define MAX_API_COUNT 3
static param_interface_t *__param_api_table__[MAX_API_COUNT] = {NULL, NULL, NULL};

int param_interface_register(param_interface_t *api) {
    if (!api) {
        return -1;
    }

    bool success = false;
    for (int i = 0; i < MAX_API_COUNT; i++) {
        if (__param_api_table__[i] == NULL) {
            __param_api_table__[i] = api;
            success                = true;
            break;
        }
    }

    return success ? 0 : -1;
}

int param_init_internal() {
    return 0;
}

uint16_t param_get_count() {
    uint16_t count = 0;
    for (size_t i = 0; i < MAX_API_COUNT; i++) {
        param_interface_t *api = __param_api_table__[i];
        if (api) {
            count += api->ops->get_count();
        }
    }
    return count;
}

param_t param_get_count_used() {
    param_t        count = 0;
    param_status_t status;

    for (param_t i = 0; i < param_get_count(); i++) {
        param_get_status(i, &status);
        if (status.actived) {
            count++;
        }
    }

    return count;
}

bool param_in_range(param_t idx) {
    return idx < param_get_count();
}

static param_interface_t *param_get_whois(param_t *idx) {
    uint16_t offset = 0;
    for (size_t i = 0; i < MAX_API_COUNT; i++) {
        param_interface_t *api = __param_api_table__[i];
        if (api) {
            if ((*idx) < offset + api->ops->get_count()) {
                (*idx) -= offset;
                return api;
            }
            offset += api->ops->get_count();
        }
    }
    return NULL;
}

int param_find_internal(const char *name, bool mark_used) {
    param_t idx    = PARAM_INVALID;
    int     offset = 0;

    for (size_t i = 0; i < MAX_API_COUNT; i++) {
        param_interface_t *api = __param_api_table__[i];

        if (api) {
            idx = api->ops->find(name, mark_used);
            if (idx != PARAM_INVALID) {
                return idx + offset;
            }

            offset += api->ops->get_count();
        }
    }

    return PARAM_INVALID;
}

param_t param_find(const char *name) {
    param_t idx = param_find_internal(name, true);
    if (idx == PARAM_INVALID) {
        LOG_W("can't find %s", name);
    }
    return idx;
}

param_t param_find_no_notification(const char *name) {
    return param_find_internal(name, false);
}

int param_get_info(param_t idx, param_info_t *info) {
    param_interface_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->get_info(idx, info);
    }

    return -1;
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

int param_set_internal(param_t idx, const param_value_t *val, bool mark_saved, bool notify) {
    param_interface_t *api = param_get_whois(&idx);

    if (!api) {
        return -1;
    }

    int ret = api->ops->set_value(idx, val, mark_saved);

    if (ret == 0) {
        if (notify) {
            param_notify_autosave();
            param_notify_changes();
        }
    }

    return ret;
}

int param_set(param_t idx, const void *val) {
    return param_set_internal(idx, (const param_value_t *)val, false, true);
}

int param_set_no_notification(param_t idx, const void *val) {
    return param_set_internal(idx, (const param_value_t *)val, false, false);
}

int param_get_internal(param_t idx, param_value_t *val, bool mark_used) {
    param_interface_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->get_value(idx, val, mark_used);
    }

    return -1;
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

int param_get_status(param_t idx, param_status_t *status) {
    param_interface_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->get_status(idx, status);
    }

    return -1;
}

int param_set_status(param_t idx, const param_status_t *status) {
    param_interface_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->set_status(idx, status);
    }

    return -1;
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

int param_reset_internal(param_t idx, bool notify) {
    param_interface_t *api = param_get_whois(&idx);

    if (!api) {
        return -1;
    }

    int ret = api->ops->reset(idx, false);

    if (ret == 0 && notify) {
        param_notify_changes();
        param_notify_autosave();
    }

    return -1;
}

int param_reset(param_t idx) {
    return param_reset_internal(idx, true);
}

int param_reset_no_notification(param_t idx) {
    return param_reset_internal(idx, false);
}

void param_reset_all_internal(bool notify) {
    for (param_t i = 0; i < param_get_count(); i++) {
        // 重置过程中不发送param_update消息
        param_reset_internal(i, false);
    }
    // 重置结束之后发送param_update消息
    if (notify) {
        param_notify_changes();
    }
}

void param_reset_all() {
    param_reset_all_internal(true);
}

void param_reset_all_notification() {
    param_reset_all_internal(false);
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
