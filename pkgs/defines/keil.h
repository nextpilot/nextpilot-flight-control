/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __NEXTPILOT_KEIL_H__
#define __NEXTPILOT_KEIL_H__

/* Define rt_isfinite */
#if defined(__cplusplus) && defined(__ARMCC_VERSION)
// KEIL在C++ 代码中isfinite / isnan有问题，因此重新定义
#define rt_isfinite(x) \
    ((sizeof(x) == sizeof(float)) ? __ARM_isfinitef(x) : __ARM_isfinite(x))
#define rt_isnan(x) \
    ((sizeof(x) == sizeof(float)) ? __ARM_isnanf(x) : __ARM_isnan(x))
#define rt_isinf(x) \
    ((sizeof(x) == sizeof(float)) ? __ARM_isinff(x) : __ARM_isinf(x))
#define rt_isnormal(x) \
    ((sizeof(x) == sizeof(float)) ? __ARM_isnormalf(x) : __ARM_isnormal(x))
#else
// 采用系统自带的函数
#define rt_isfinite(x) (isfinite(x))
#define rt_isnan(x)    (isnan(x))
#define rt_isinf(x)    (isinf(x))
#define rt_isnormal(x) (isnormal(x))
#endif /* __cplusplus && __ARMCC_VERSION */

#endif // __NEXTPILOT_KEIL_H__