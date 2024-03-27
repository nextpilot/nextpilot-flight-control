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

#include "param_common.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct param_interface_ops_s {
    int (*init)();
    param_t (*find)(const char *name, bool mark_used);
    uint16_t (*get_count)();
    int (*get_info)(param_t idx, param_info_t *info);
    int (*get_value)(param_t idx, param_value_t *val, bool mark_used);
    int (*set_value)(param_t idx, const param_value_t *val, bool mark_saved);
    int (*get_status)(param_t idx, param_status_t *status);
    int (*set_status)(param_t idx, const param_status_t *status);
    int (*reset)(param_t idx, bool mark_saved);
} param_interface_ops_t;

typedef struct param_interface_s {
    const char            *name;
    uint8_t                type;
    param_interface_ops_t *ops;
} param_interface_t;

int param_interface_register(param_interface_t *api);

uint16_t param_get_count();
uint16_t param_get_count_used();
bool     param_in_range(param_t idx);

// 查找参数
int     param_find_internal(const char *name, bool mark_used);
param_t param_find(const char *name);

// 获取meta信息
int          param_get_info(param_t idx, param_info_t *info);
const char  *param_get_name(param_t idx);
uint8_t      param_get_size(param_t idx);
const char  *param_get_type_cstr(param_t idx);
param_type_t param_get_type(param_t idx);
param_flag_t param_get_flag(param_t idx);
int          param_get_default_value(param_t idx, param_value_t *val);

// 获取参数
int     param_get_internal(param_t idx, param_value_t *val, bool mark_used);
int     param_get(param_t idx, void *val);
float   param_get_float(param_t idx);
int32_t param_get_int32(param_t idx);

// 设置参数
int param_set_internal(param_t idx, const param_value_t *val, bool mark_saved, bool notify_changes);
int param_set(param_t idx, const void *val);
int param_set_no_notification(param_t idx, const void *val);
int param_set_float(param_t idx, float val);
int param_set_int32(param_t idx, int32_t val);

// 设置/查询状态
param_status_t param_get_status(param_t idx);
int            param_set_status(param_t idx, const param_status_t *status);
bool           param_value_unsaved(param_t idx);
bool           param_value_used(param_t idx);
bool           param_value_changed(param_t idx);
// int  param_set_used(param_t idx);
// int  param_set_saved(param_t idx);
// int  param_set_saved_ulog(param_t idx);

// 重置参数
int  param_reset_internal(param_t idx, bool notify);
int  param_reset(param_t idx);
int  param_reset_no_notification(param_t idx);
void param_reset_all_internal(bool notify);
void param_reset_all();
void param_reset_all_no_notification();
void param_reset_excludes(const char *excludes[], int num_excludes);
void param_reset_specific(const char *resets[], int num_resets);

#ifdef __cplusplus
}
#endif //__cplusplus __cplusplus

#endif // __PARAM_INTERFACE_H__
