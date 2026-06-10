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
 * @file drv_led.h
 *
 * Led device API to control the external LED(s) via uORB interface
 */

#pragma once


#include <uORB/topics/led_control.h>

// #include <board_config.h>

// allow the board to override the number (or maximum number) of LED's it has
#ifndef BOARD_MAX_LEDS
#define BOARD_MAX_LEDS 4
#endif

static_assert(led_control_s::ORB_QUEUE_LENGTH >= BOARD_MAX_LEDS, "led_control_s::ORB_QUEUE_LENGTH too small");

#if BOARD_MAX_LEDS > 8 // because led_mask is uint8_t
#error "BOARD_MAX_LEDS too large. You need to change the led_mask type in the led_control uorb topic (and where it's used)"
#endif
