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
#include "param_common.h"
#include "param_interface.h"
#include "param_storage.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// 查找参数
static param_t param_find(const char *name) {
    param_t idx = param_find_internal(name, true);
    if (idx == PARAM_INVALID) {
        // LOG_W("can't find %s", name);
    }
    return idx;
}

// 获取meta信息
const char   *param_get_name(param_t idx);
uint8_t       param_get_size(param_t idx);
const char   *param_get_type_cstr(param_t idx);
param_type_t  param_get_type(param_t idx);
param_flag_t  param_get_flag(param_t idx);
param_value_t param_get_default_value(param_t idx);

// const char *param_type_cstr(param_type_t type);
// uint8_t     param_type_size(param_type_t type);

// 获取参数
int     param_get(param_t idx, void *val);
float   param_get_float(param_t idx);
int32_t param_get_int32(param_t idx);

// 设置参数
int param_set(param_t idx, const void *val);
int param_set_no_notification(param_t idx, const void *val);
int param_set_float(param_t idx, float val);
int param_set_int32(param_t idx, int32_t val);
// int param_set_from_load(param_t idx, const void *val);

// 设置/查询状态
int param_set_used(param_t idx);
int param_set_saved(param_t idx);
int param_set_saved_ulog(param_t idx);

// 重置参数
int  param_reset(param_t idx);
int  param_reset_no_notification(param_t idx);
int  param_reset_all(bool autosave);
void param_reset_excludes(const char *excludes[], int num_excludes);
void param_reset_specific(const char *resets[], int num_resets);

// HASH计算
int param_calc_hash();
int param_cacl_hash_specific();

// 加载保存

// 打印参数
void param_print(param_t idx, bool show_header);

// 辅助函数
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__PARAM_H__
