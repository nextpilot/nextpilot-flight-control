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
 * @file drv_io_heater.h
 *
 * IO IMU heater control interface (PX4IO)
 */

#ifndef _DRV_IO_HEATER_H
#define _DRV_IO_HEATER_H

#include <stdint.h>
#include <sys/ioctl.h>

/*
 * ioctl() definitions
 */

#define IO_HEATER_DEVICE_PATH "/dev/px4io"

#define _IO_HEATER_BASE       (0x2e00)

#define PX4IO_HEATER_CONTROL  _PX4_IOC(_IO_HEATER_BASE, 0)

/* ... to IOX_SET_VALUE + 8 */

/* enum passed to IO_HEATER_CONTROL ioctl()*/
enum IO_HEATER_MODE {
	HEATER_MODE_OFF = 0,
	HEATER_MODE_ON = 1,
	HEATER_MODE_DISABLED = -1
};

#endif
