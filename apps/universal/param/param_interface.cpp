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
#include "param_interface.h"

#define MAX_API_COUNT 3
static param_api_t *__param_api_table__[MAX_API_COUNT] = {NULL, NULL, NULL};

int param_api_register(param_api_t *api) {
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
        param_api_t *api = __param_api_table__[i];
        if (api) {
            count += api->ops->get_count();
        }
    }
    return count;
}

bool param_in_range(param_t idx) {
    return idx < param_get_count();
}

static param_api_t *param_get_whois(param_t *idx) {
    uint16_t offset = 0;
    for (size_t i = 0; i < MAX_API_COUNT; i++) {
        param_api_t *api = __param_api_table__[i];
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
        param_api_t *api = __param_api_table__[i];

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

int param_get_info(param_t idx, param_info_t *info) {
    param_api_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->get_info(idx, info);
    }

    return -1;
}

int param_set_internal(param_t idx, const param_value_t *val, bool mark_saved, bool notify_changes) {
    param_api_t *api = param_get_whois(&idx);

    if (!api) {
        return -1;
    }

    int ret = api->ops->set_value(idx, val, mark_saved);

    if (ret == 0 && notify_changes) {
        param_notify_changes();
    }

    return ret;
}

int param_get_internal(param_t idx, param_value_t *val, bool mark_used) {
    param_api_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->get_value(idx, val, mark_used);
    }

    return -1;
}

int param_get_status(param_t idx, param_status_t *status) {
    param_api_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->get_status(idx, status);
    }

    return -1;
}

int param_set_status(param_t idx, param_status_t *status) {
    param_api_t *api = param_get_whois(&idx);

    if (api) {
        return api->ops->set_status(idx, status);
    }

    return -1;
}

int param_reset_internal(param_t idx, bool notify_changes) {
    param_api_t *api = param_get_whois(&idx);

    if (!api) {
        return -1;
    }

    int ret = api->ops->reset(idx);

    if (ret == 0 && notify_changes) {
        param_notify_changes();
    }

    return -1;
}
