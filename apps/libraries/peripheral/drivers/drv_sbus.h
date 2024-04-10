/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file drv_sbus.h
 *
 * Futaba S.BUS / S.BUS 2 compatible interface.
 */

#ifndef _DRV_SBUS_H
#define _DRV_SBUS_H

#include <stdint.h>
#include <sys/ioctl.h>

#include "drv_orb_dev.h"

/**
 * Path for the default S.BUS device
 */
#define SBUS0_DEVICE_PATH	"/dev/sbus0"

#define _SBUS_BASE		0x2c00

/** Enable S.BUS version 1 / 2 output (0 to disable) */
#define SBUS_SET_PROTO_VERSION		_IOC(_SBUS_BASE, 0)

#endif /* _DRV_SBUS_H */
