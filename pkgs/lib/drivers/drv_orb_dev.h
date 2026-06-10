/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef _DRV_UORB_H
#define _DRV_UORB_H

/**
 * @file drv_orb_dev.h
 *
 * uORB published object driver.
 */

#include <defines.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>

#define _ORBIOCBASE		(0x2600)
#define _ORBIOC(_n)		(_PX4_IOC(_ORBIOCBASE, _n))

/*
 * IOCTLs for individual topics.
 */

/** Check whether the topic has been updated since it was last read, sets *(bool *)arg */
#define ORBIOCUPDATED		_ORBIOC(11)

/** Set the minimum interval at which the topic can be seen to be updated for this subscription */
#define ORBIOCSETINTERVAL	_ORBIOC(12)

/** Get the global advertiser handle for the topic */
#define ORBIOCGADVERTISER	_ORBIOC(13)

/** Get the priority for the topic */
#define ORBIOCGPRIORITY		_ORBIOC(14)

/** Set the queue size of the topic */
#define ORBIOCSETQUEUESIZE	_ORBIOC(15)

/** Get the minimum interval at which the topic can be seen to be updated for this subscription */
#define ORBIOCGETINTERVAL	_ORBIOC(16)

/** Check whether the topic is advertised, sets *(unsigned long *)arg to 1 if advertised, 0 otherwise */
#define ORBIOCISADVERTISED	_ORBIOC(17)

#endif /* _DRV_UORB_H */
