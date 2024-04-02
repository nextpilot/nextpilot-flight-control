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

/* Exponential and Logarithmic constants */
#define M_E       2.7182818284590452353602874713526625 // e
#define M_SQRT2   1.4142135623730950488016887242096981 // sqrt(2)
#define M_SQRT1_2 0.7071067811865475244008443621048490 // 1/sqrt(2)
#define M_LOG2E   1.4426950408889634073599246810018921 // log2(e)
#define M_LOG10E  0.4342944819032518276511289189166051 // lo
#define M_LN2     0.6931471805599453094172321214581765
#define M_LN10    2.3025850929940456840179914546843642

/* Trigonometric Constants */
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795029
#endif

#ifndef M_PI_2
#define M_PI_2 1.5707963267948966192313216916397514
#endif

#ifndef M_PI_4
#define M_PI_4 0.7853981633974483096156608458198757
#endif

#ifndef M_1_PI
#define M_1_PI 0.3183098861837906715377675267450287
#endif

#ifndef M_2_PI
#define M_2_PI 0.6366197723675813430755350534900574
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.1283791670955125738961589031215452
#endif

#ifndef M_PI_F
#define M_PI_F ((float)M_PI)
#endif

#ifndef M_PI_2_F
#define M_PI_2_F ((float)M_PI_2)
#endif

#ifndef M_DEG_TO_RAD
#define M_DEG_TO_RAD 0.017453292519943295
#endif

#ifndef M_RAD_TO_DEG
#define M_RAD_TO_DEG 57.295779513082323
#endif

#ifndef M_PI_F
#define M_PI_F 3.14159265358979323846f
#endif

// #ifndef CONSTANTS_ONE_G
// #ifndef LIB_USING_ECL_GEO
// #define CONSTANTS_ONE_G 9.80665 // m/s^2
// #endif
// #endif

#endif // __NEXTPILOT_CONSTANTS_H__
