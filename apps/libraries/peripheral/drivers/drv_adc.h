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
#include <uORB/topics/adc_report.h>

/* Define the PX4 low level format ADC and the maximum
 * number of channels that can be returned by a lowlevel
 * ADC driver. Drivers may return less than PX4_MAX_ADC_CHANNELS
 * but no more than PX4_MAX_ADC_CHANNELS.
 *
 */
#define PX4_MAX_ADC_CHANNELS arraySize(adc_report_s::channel_id)
typedef struct __attribute__((packed)) px4_adc_msg_t {
	uint8_t      am_channel;               /* The 8-bit ADC Channel */
	int32_t      am_data;                  /* ADC convert result (4 bytes) */
} px4_adc_msg_t;


#define BUILTIN_ADC_DEVID	0xffffffff	// TODO: integrate into existing ID management


__BEGIN_DECLS

/**
 * Initialize ADC hardware
 * @param base_address architecture-specific address to specify the ADC
 * @return 0 on success, <0 error otherwise
 */
int px4_arch_adc_init(uint32_t base_address);

/**
 * Uninitialize ADC hardware
 * @param base_address architecture-specific address to specify the ADC
 */
void px4_arch_adc_uninit(uint32_t base_address);

/**
 * Read a sample from the ADC
 * @param base_address architecture-specific address to specify the ADC
 * @param channel specify the channel
 * @return sample, 0xffffffff on error
 */
uint32_t px4_arch_adc_sample(uint32_t base_address, unsigned channel);

/**
 * Get the ADC positive reference voltage
 * N.B This assume that all ADC channels share the same vref.
 * @return v_ref
 */
float px4_arch_adc_reference_v(void);

/**
 * Get the temperature sensor channel bitmask
 */
uint32_t px4_arch_adc_temp_sensor_mask(void);

/**
 * Get the adc digital number full count
 */
uint32_t px4_arch_adc_dn_fullcount(void);

__END_DECLS
