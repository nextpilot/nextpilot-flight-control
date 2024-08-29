/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __BATTERY_CONFIG_H__
#define __BATTERY_CONFIG_H__

// #define BOARD_NUMBER_BRICKS          1
// #define ADC_BATTERY_DEVICE_NAME      "adc1"
// #define ADC_BATTERY1_VOLTAGE_CHANNEL 0 /* PA0 */
// #define ADC_BATTERY1_CURRENT_CHANNEL 1 /* PA1 */
// #define ADC_BATTERY2_VOLTAGE_CHANNEL 2 /* PA2 */
// #define ADC_BATTERY2_CURRENT_CHANNEL 3 /* PA3 */

#define BOARD_VALID_UV           (3.7f)
#define BOARD_ADC_OPEN_CIRCUIT_V (5.6f)
#define BOARD_BATTERY1_V_DIV     (21.0f)
#define BOARD_BATTERY1_A_PER_V   (36.367515152f)
#define BOARD_BATTERY2_V_DIV     (21.0f)
#define BOARD_BATTERY2_A_PER_V   (36.367515152f)

#endif // __BATTERY_CONFIG_H__
