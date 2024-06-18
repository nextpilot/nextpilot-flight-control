/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __NEXTPILOT_CONSTANTS_H__
#define __NEXTPILOT_CONSTANTS_H__

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
#include <math.h>
// static inline constexpr bool rt_isfinite(float x) {
//     return __builtin_isfinite(x);
// }
// static inline constexpr bool rt_isfinite(double x) {
//     return __builtin_isfinite(x);
// }
// 采用系统自带的函数
#define rt_isfinite(x) (isfinite(x))
#define rt_isnan(x)    (isnan(x))
#define rt_isinf(x)    (isinf(x))
#define rt_isnormal(x) (isnormal(x))
#endif /* __cplusplus && __ARMCC_VERSION */

/* Math macro's for float literals. Do not use M_PI et al as they aren't
 * defined (neither C nor the C++ standard define math constants) */
#define M_E_F          2.71828183f
#define M_LOG2E_F      1.44269504f
#define M_LOG10E_F     0.43429448f
#define M_LN2_F        0.69314718f
#define M_LN10_F       2.30258509f
#define M_PI_F         3.14159265f
#define M_TWOPI_F      6.28318531f
#define M_PI_2_F       1.57079632f
#define M_PI_4_F       0.78539816f
#define M_3PI_4_F      2.35619449f
#define M_SQRTPI_F     1.77245385f
#define M_1_PI_F       0.31830989f
#define M_2_PI_F       0.63661977f
#define M_2_SQRTPI_F   1.12837917f
#define M_DEG_TO_RAD_F 0.0174532925f
#define M_RAD_TO_DEG_F 57.2957795f
#define M_SQRT2_F      1.41421356f
#define M_SQRT1_2_F    0.70710678f
#define M_LN2LO_F      1.90821484E-10f
#define M_LN2HI_F      0.69314718f
#define M_SQRT3_F      1.73205081f
#define M_IVLN10_F     0.43429448f // 1 / log(10)
#define M_LOG2_E_F     0.69314718f
#define M_INVLN2_F     1.44269504f // 1 / log(2)

/* The M_PI, as stated above, is not C standard. If you need it and
 * it isn't in your math.h file then you can use this instead. */
#define M_PI_PRECISE 3.141592653589793238462643383279502884

#define M_DEG_TO_RAD 0.017453292519943295
#define M_RAD_TO_DEG 57.295779513082323

#endif // __NEXTPILOT_CONSTANTS_H__
