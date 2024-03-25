/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "param_global_autogen.h"
#include "rtthread.h"
#include "rtdbg.h"
#include <cstring>

namespace nextpilot::global_params {

static constexpr uint16_t  __param_count__ = (uint16_t)params::MAX_COUNT;
static const param_info_s *__param_info__  = infos;
static param_data_t        __param_data__[__param_count__]{};

uint16_t param_get_count() {
    return __param_count__;
}

constexpr bool param_in_range(uint16_t idx) {
    return idx < __param_count__;
}

param_t param_find(const char *name, bool mark_used) {
    if (!name) {
        return PARAM_INVALID;
    }

    param_t middle = 0;
    param_t front  = 0;
    param_t last   = param_get_count();

    /* perform a binary search of the known parameters */

    while (front <= last) {
        middle  = front + (last - front) / 2;
        int ret = strcmp(name, __param_info__[middle].name);

        if (ret == 0) {
            if (mark_used) {
                rt_enter_critical();
                __param_data__[middle].status.actived = 1;
                rt_exit_critical();
            }
            return middle;
        } else if (middle == front) {
            /* An end point has been hit, but there has been no match */
            break;
        } else if (ret < 0) {
            last = middle;
        } else {
            front = middle;
        }
    }

    return PARAM_INVALID;
}

const char *param_get_name(uint16_t idx) {
    return param_in_range(idx) ? __param_info__[idx].name : nullptr;
}

param_type_t param_get_type(uint16_t idx) {
    return param_in_range(idx) ? __param_info__[idx].type : PARAM_TYPE_UNKNOWN;
}

uint8_t param_get_size(uint16_t idx) {
    return param_in_range(idx) ? param_type_size(__param_info__[idx].type) : 0;
}

param_flag_t param_get_flag(uint16_t idx) {
    param_flag_t flag = {.value = 0};
    return param_in_range(idx) ? __param_info__[idx].flag : flag;
}

int param_get_default_value(uint16_t idx, void *val) {
    if (!param_in_range(idx) || !val) {
        return -1;
    }

    memcpy(val, &__param_info__[idx].value, param_get_size(idx));

    // 标记为已经使用
    rt_enter_critical();
    __param_data__[idx].status.actived = 1;
    rt_exit_critical();
    return 0;
}

param_status_t param_get_status(param_t idx) {
    param_status_t status = {.value = 0};
    if (param_in_range(idx)) {
        rt_enter_critical();
        status = __param_data__[idx].status;
        rt_exit_critical();
    }
    return status;
}

int param_set_status(param_t idx, param_status_t status) {
    if (!param_in_range(idx)) {
        return -1;
    }

    rt_enter_critical();
    __param_data__[idx].status = status;
    rt_exit_critical();
    return 0;
}

int param_get_value(param_t idx, void *val, bool mark_used) {
    if (!val || !param_in_range(idx)) {
        LOG_E("param idx %d out range %d", idx, param_get_count());
        return -1;
    }

    rt_enter_critical();
    if (__param_data__[idx].status.changed) {
        memcpy(val, &__param_data__[idx].value, param_get_size(idx));
    } else {
        memcpy(val, &__param_info__[idx].value, param_get_size(idx));
    }
    // 标记为已经使用
    if (mark_used) {
        __param_data__[idx].status.actived = 1;
    }
    rt_exit_critical();

    return 0;
}

int param_set_value(param_t idx, const void *val, bool mark_saved, bool notify_changes) {
    if (!param_in_range(idx) || !val) {
        return -1;
    }

    rt_enter_critical();
    memcpy(&__param_data__[idx].value, val, param_get_size(idx));
    // 标记状态
    __param_data__[idx].status.changed         = 1;
    __param_data__[idx].status.unsaved         = !mark_saved;
    __param_data__[idx].status.unsaved_to_ulog = !mark_saved;
    rt_exit_critical();

    // if (!mark_saved) {
    //     rt_event_send(&_param_changed_event, EVENT_PARAM_CHANGED);
    // }

    if (notify_changes) {
        param_notify_changes();
    }

    return 0;
}

} // namespace nextpilot::global_params
