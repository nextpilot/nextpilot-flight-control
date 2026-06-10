/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/*
 * @file circuit_breaker.h
 *
 * Circuit breaker functionality.
 */

#ifndef CIRCUIT_BREAKER_H_
#define CIRCUIT_BREAKER_H_

/* SAFETY WARNING  --  SAFETY WARNING  --  SAFETY WARNING
 *
 * OBEY THE DOCUMENTATION FOR ALL CIRCUIT BREAKERS HERE,
 * ENSURE TO READ CAREFULLY ALL SAFETY WARNINGS.
 * http://pixhawk.org/dev/circuit_breakers
 *
 * CIRCUIT BREAKERS ARE NOT PART OF THE STANDARD OPERATION PROCEDURE
 * AND MAY DISABLE CHECKS THAT ARE VITAL FOR SAFE FLIGHT.
 */

#define CBRK_BUZZER_KEY     782097
#define CBRK_SUPPLY_CHK_KEY 894281
#define CBRK_IO_SAFETY_KEY  22027
#define CBRK_AIRSPD_CHK_KEY 162128
#define CBRK_FLIGHTTERM_KEY 121212
#define CBRK_USB_CHK_KEY    197848
#define CBRK_VTOLARMING_KEY 159753

#include <stdint.h>

extern "C" bool circuit_breaker_enabled(const char *breaker, int32_t magic);

static inline bool circuit_breaker_enabled_by_val(int32_t breaker_val, int32_t magic) {
    return breaker_val == magic;
}

#endif /* CIRCUIT_BREAKER_H_ */
