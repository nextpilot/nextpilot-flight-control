/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_INTERFACE_H__
#define __PARAM_INTERFACE_H__

#include <stdint.h>
#include <stdbool.h>
#include "../param_common.h"

typedef struct param_api_ops_s {
    int (*init)();
    int (*find)(const char *name, bool mark_used);
    uint16_t (*get_count)();
    int (*get_info)(param_t idx, const param_info_t *info);
    int (*get_value)(param_t idx, const param_value_t *val, bool mark_used);
    int (*set_value)(param_t idx, const param_value_t *val, bool mark_saved);
    int (*get_status)(param_t idx, const param_status_t *status);
    int (*set_status)(param_t idx, param_status_t *status);
    int (*reset)(param_t idx);
} param_api_ops_t;

typedef struct param_api_s {
    const char      *name;
    uint8_t          type;
    param_api_ops_t *ops;
} param_api_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint16_t param_get_count();
bool     param_in_range(param_t idx);

int param_find_internal(const char *name, bool mark_used);

int param_get_info(param_t idx, param_info_t *info);
int param_get_status(param_t idx, param_status_t *status);
int param_set_status(param_t idx, param_status_t *status);

int param_set_internal(param_t idx, const param_value_t *val, bool mark_saved, bool notify_changes);
int param_get_internal(param_t idx, param_value_t *val, bool mark_used);

int param_reset_internal(param_t idx, bool notify_changes);

#ifdef __cplusplus
}
#endif //__cplusplus __cplusplus

#endif // __PARAM_INTERFACE_H__
