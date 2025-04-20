/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <px4_platform_common/micro_hal.h>

#include "board_config.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#if defined(GPIO_OTGFS_VBUS) && (defined(CONFIG_BUILD_FLAT) || !defined(__PX4_NUTTX))

/* Default implementation for POSIX and flat NUTTX if the VBUS pin exists */
int board_read_VBUS_state(void) {
    return (px4_arch_gpioread(GPIO_OTGFS_VBUS) ? 0 : 1);
}
#endif
