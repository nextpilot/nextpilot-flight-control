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
 * @file commander_helper.h
 * Commander helper functions definitions
 *
 * @author Thomas Gubler <thomasgubler@student.ethz.ch>
 * @author Julian Oes <julian@oes.ch>
 */

#ifndef COMMANDER_HELPER_H_
#define COMMANDER_HELPER_H_

#include <uORB/uORB.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/actuator_armed.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <drivers/drv_led.h>
#include <drivers/drv_board_led.h>

bool is_multirotor(const vehicle_status_s &current_status);
bool is_rotary_wing(const vehicle_status_s &current_status);
bool is_vtol(const vehicle_status_s &current_status);
bool is_vtol_tailsitter(const vehicle_status_s &current_status);
bool is_fixed_wing(const vehicle_status_s &current_status);
bool is_ground_vehicle(const vehicle_status_s &current_status);

int  buzzer_init(void);
void buzzer_deinit(void);

/**
 * @brief Override tune control and play the given tune
 */
void set_tune_override(const int tune_id);

/**
 * @brief Set the new tune to play under predefined conditions
 *
 * Setting a new tune will only be possible for the following 3 cases:
 *
 * 1. Current playing tune is non-repeating and has ended
 * 2. Current playing tune is non-repeating, but the requested tune is also non-repeating, and they are different tunes
 * 3. Current playing tune is repeating, and the requested tune is different from the current tune
 *
 * This is to prevent repeating tunes from overriding single-play tunes.
 */
void set_tune(const int tune_id);

void tune_home_set(bool use_buzzer);
void tune_mission_ok(bool use_buzzer);
void tune_mission_warn(bool use_buzzer);
void tune_mission_fail(bool use_buzzer);
void tune_positive(bool use_buzzer);
void tune_neutral(bool use_buzzer);
void tune_negative(bool use_buzzer);
void tune_failsafe(bool use_buzzer);

int blink_msg_state();

/* methods to control the onboard LED(s) */
int  led_init(void);
void led_deinit(void);
int  led_toggle(int led);
int  led_on(int led);
int  led_off(int led);

/**
 * set the LED color & mode
 * @param color @see led_control_s::COLOR_*
 * @param mode @see led_control_s::MODE_*
 */
void rgbled_set_color_and_mode(uint8_t color, uint8_t mode);

void rgbled_set_color_and_mode(uint8_t color, uint8_t mode, uint8_t blinks, uint8_t prio);

#endif /* COMMANDER_HELPER_H_ */
