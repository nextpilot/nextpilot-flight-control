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
 * @file drv_board_led.h
 *
 * LED driver API to control the onboard LED(s) via ioctl() interface
 */

#pragma once

#include <defines.h>
#include <stdint.h>
#include <sys/ioctl.h>

#define LED_BASE_DEVICE_PATH		"/dev/led"
#define LED0_DEVICE_PATH		"/dev/led0"

#define _LED_BASE		0x2800

/* PX4 LED colour codes */
#define LED_AMBER		1
#define LED_RED			1	/* some boards have red rather than amber */
#define LED_BLUE		0
#define LED_SAFETY		2
#define LED_GREEN		3


#define LED_ON			_PX4_IOC(_LED_BASE, 0)
#define LED_OFF			_PX4_IOC(_LED_BASE, 1)
#define LED_TOGGLE		_PX4_IOC(_LED_BASE, 2)

__BEGIN_DECLS

/*
 * Initialise the LED driver.
 */
__EXPORT void drv_led_start(void);

__END_DECLS
