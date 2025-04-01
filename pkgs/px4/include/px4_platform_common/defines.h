/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2025 NextPilot Development Team
 ******************************************************************/


#ifndef __DEFINES_H__
#define __DEFINES_H__

#define PX4_ERROR (-1)
#define PX4_OK    (0)


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


#define PX4_ROOTFSDIR  ""
#define PX4_STORAGEDIR PX4_ROOTFSDIR "/fs/microsd"


// mode for open with O_CREAT
#define PX4_O_MODE_777 0777
#define PX4_O_MODE_666 0666
#define PX4_O_MODE_600 0600

#endif // __DEFINES_H__
