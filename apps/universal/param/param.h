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
