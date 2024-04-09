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
 * @file led.cpp
 *
 * LED driver to control the onboard LED(s) via ioctl interface.
 */

#include <px4_platform_common/log.h>
#include <px4_platform_common/px4_config.h>
#include <lib/cdev/CDev.hpp>
#include <drivers/drv_board_led.h>
#include <stdio.h>

/*
 * Ideally we'd be able to get these from arm_internal.h,
 * but since we want to be able to disable the NuttX use
 * of leds for system indication at will and there is no
 * separate switch, we need to build independent of the
 * CONFIG_ARCH_LEDS configuration switch.
 */
__BEGIN_DECLS
extern void led_init();
extern void led_on(int led);
extern void led_off(int led);
extern void led_toggle(int led);
__END_DECLS

class LED : cdev::CDev
{
public:
	LED();
	~LED() override = default;

	int	init() override;
	int	ioctl(cdev::file_t *filp, int cmd, unsigned long arg) override;
};

LED::LED() : CDev(LED0_DEVICE_PATH)
{
	// force immediate init/device registration
	init();
}

int
LED::init()
{
	PX4_DEBUG("LED::init");

	CDev::init();

	led_init();

	return 0;
}

int
LED::ioctl(cdev::file_t *filp, int cmd, unsigned long arg)
{
	int result = OK;

	switch (cmd) {
	case LED_ON:
		led_on(arg);
		break;

	case LED_OFF:
		led_off(arg);
		break;

	case LED_TOGGLE:
		led_toggle(arg);
		break;

	default:
		result = CDev::ioctl(filp, cmd, arg);
	}

	return result;
}

namespace
{
LED	*gLED;
}

void
drv_led_start(void)
{
	if (gLED == nullptr) {
		gLED = new LED;
	}
}
