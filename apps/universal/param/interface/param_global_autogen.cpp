/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <rtthread.h>
#include <rtdbg.h>
#include <string.h>
#include "param_interface.h"
#include "param_global_autogen.h"

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

int param_get_info(param_t idx, param_info_t *info) {
    if (!info || !param_in_range(idx)) {
        return -1;
    }

    info->name  = __param_info__[idx].name;
    info->type  = __param_info__[idx].type;
    info->value = __param_info__[idx].value;
    info->flag  = __param_info__[idx].flag;
    return 0;
}

int param_get_status(param_t idx, param_status_t *status) {
    if (!status || !param_in_range(idx)) {
        return -1;
    }

    rt_enter_critical();
    *status = __param_data__[idx].status;
    rt_exit_critical();
    return 0;
}

int param_set_status(param_t idx, const param_status_t *status) {
    if (!status || !param_in_range(idx)) {
        return -1;
    }

    rt_enter_critical();
    __param_data__[idx].status = (*status);
    rt_exit_critical();
    return 0;
}

uint8_t param_get_size(param_t idx) {
    if (param_in_range(idx)) {
        return param_type_size(__param_info__[idx].type);
    }
    return 0;
}

int param_get_value(param_t idx, param_value_t *val, bool mark_used) {
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

int param_set_value(param_t idx, const param_value_t *val, bool notify) {
    if (!param_in_range(idx) || !val) {
        return -1;
    }

    rt_enter_critical();
    memcpy(&__param_data__[idx].value, val, param_get_size(idx));
    // 标记状态
    __param_data__[idx].status.changed         = true;
    __param_data__[idx].status.unsaved         = true;
    __param_data__[idx].status.unsaved_to_ulog = true;
    rt_exit_critical();

    if (notify) {
        // param_notify_changes();
    }

    return 0;
}

int param_reset(param_t idx, bool mark_saved) {
    if (!param_in_range(idx)) {
        return -1;
    }

    rt_enter_critical();
    __param_data__[idx].status.value = 0;
    rt_exit_critical();

    return 0;
}

} // namespace nextpilot::global_params

static param_interface_ops_t _ops = {
    .init       = NULL,
    .find       = nextpilot::global_params::param_find,
    .get_count  = nextpilot::global_params::param_get_count,
    .get_info   = nextpilot::global_params::param_get_info,
    .get_value  = nextpilot::global_params::param_get_value,
    .set_value  = nextpilot::global_params::param_set_value,
    .get_status = nextpilot::global_params::param_get_status,
    .set_status = nextpilot::global_params::param_set_status,
    .reset      = nextpilot::global_params::param_reset,
};

static param_interface_t _itf = {
    .name = "global",
    .type = 0,
    .ops  = &_ops,
};

static int param_global_autogen_init() {
    return param_interface_register(&_itf);
}
INIT_COMPONENT_EXPORT(param_global_autogen_init);
