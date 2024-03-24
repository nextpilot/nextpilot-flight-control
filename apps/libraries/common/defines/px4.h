/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __NEXTPILOT_PX4DEF_H__
#define __NEXTPILOT_PX4DEF_H__

#define PX4_INFO     LOG_I
#define PX4_PANIC    LOG_I
#define PX4_ERR      LOG_E
#define PX4_WARN     LOG_W
#define PX4_DEBUG    LOG_D
#define PX4_INFO_RAW LOG_RAW

#define PX4_ERROR (-1)
#define PX4_OK    0

#ifndef ERROR
// #define ERROR (-1)
#endif // ERROR

#ifndef OK
#define OK 0
#endif // OK

#define px4_usleep usleep

#ifndef PX4_ISFINITE
#define PX4_ISFINITE rt_isfinite
#endif // PX4_ISFINITE

#endif // __NEXTPILOT_PX4DEF_H__