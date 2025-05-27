/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2025 NextPilot Development Team
 ******************************************************************/


#ifndef __PX4_PLATFORM_DEFINES_H__
#define __PX4_PLATFORM_DEFINES_H__

#define PX4_ERROR (-1)
#define PX4_OK    (0)

#ifndef ERROR
// #define ERROR (-1)
#endif // ERROR

#ifndef OK
#   define OK 0
#endif // OK


/* Define PX4_ISFINITE */
#ifdef __cplusplus
static inline constexpr bool PX4_ISFINITE(float x) {
    return __builtin_isfinite(x);
}

static inline constexpr bool PX4_ISFINITE(double x) {
    return __builtin_isfinite(x);
}
#else
#   define PX4_ISFINITE(x) __builtin_isfinite(x)
#endif /* __cplusplus */


#ifndef SYS_USING_ROMFS
#   define PX4_ROOTFSDIR  ""
#   define PX4_STORAGEDIR ""
#else
#   define PX4_ROOTFSDIR  ""
#   define PX4_STORAGEDIR PX4_ROOTFSDIR "/fs/microsd"
#endif


// mode for open with O_CREAT
#define PX4_O_MODE_777 0777
#define PX4_O_MODE_666 0666
#define PX4_O_MODE_600 0600

#endif // __PX4_PLATFORM_DEFINES_H__
