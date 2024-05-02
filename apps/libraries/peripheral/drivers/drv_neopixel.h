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
 * @file drv_adc.h
 *
 * ADC driver interface.
 *
 */

#pragma once

#include <stdint.h>
#include <sys/ioctl.h>

namespace neopixel
{
class NeoLEDData
{
public:
	enum eRGB {
		eB = 0,
		eR = 1,
		eG = 2
	};

	typedef union {
		uint8_t  grb[3];
		uint32_t l;
	} led_data_t;

	led_data_t  data{};
	NeoLEDData() {data.l = 0;}
	NeoLEDData(NeoLEDData &r) {data.l = r.data.l;}

	uint8_t &R() {return data.grb[eR];};
	uint8_t &G() {return data.grb[eG];};
	uint8_t &B() {return data.grb[eB];};
};
};

__BEGIN_DECLS

int neopixel_init(neopixel::NeoLEDData *led_data, int number_of_packages);
int neopixel_write(neopixel::NeoLEDData *led_data, int number_of_packages);
int neopixel_deinit(void);
int neopixel_write_no_dma(uint8_t r, uint8_t g, uint8_t b, uint8_t led_count);
__END_DECLS
