/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
#define PARAM_DEFINE_INT32(...)

/*
 * @file circuit_breaker.c
 *
 * Circuit breaker parameters.
 * Analog to real aviation circuit breakers these parameters
 * allow to disable subsystems. They are not supported as standard
 * operation procedure and are only provided for development purposes.
 * To ensure they are not activated accidentally, the associated
 * parameter needs to set to the key (magic).
 */

/**
 * Circuit breaker for power supply check
 *
 * Setting this parameter to 894281 will disable the power valid
 * checks in the commander.
 * WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK
 *
 * @min 0
 * @max 894281
 * @category Developer
 * @group Circuit Breaker
 */
PARAM_DEFINE_INT32(CBRK_SUPPLY_CHK, 0);

/**
 * Circuit breaker for IO safety
 *
 * Setting this parameter to 22027 will disable IO safety.
 * WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK
 *
 * @min 0
 * @max 22027
 * @category Developer
 * @group Circuit Breaker
 */
PARAM_DEFINE_INT32(CBRK_IO_SAFETY, 22027);

/**
 * Circuit breaker for airspeed sensor
 *
 * Setting this parameter to 162128 will disable the check for an airspeed sensor.
 * The sensor driver will not be started and it cannot be calibrated.
 * WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK
 *
 * @reboot_required true
 * @min 0
 * @max 162128
 * @category Developer
 * @group Circuit Breaker
 */
PARAM_DEFINE_INT32(CBRK_AIRSPD_CHK, 0);

/**
 * Circuit breaker for flight termination
 *
 * Setting this parameter to 121212 will disable the flight termination action if triggered
 * by the FailureDetector logic or if FMU is lost.
 * This circuit breaker does not affect the RC loss, data link loss, geofence,
 * and takeoff failure detection safety logic.
 *
 * @reboot_required true
 * @min 0
 * @max 121212
 * @category Developer
 * @group Circuit Breaker
 */
PARAM_DEFINE_INT32(CBRK_FLIGHTTERM, 121212);

/**
 * Circuit breaker for disabling buzzer
 *
 * Setting this parameter to 782097 will disable the buzzer audio notification.
 *
 * Setting this parameter to 782090 will disable the startup tune, while keeping
 * all others enabled.
 *
 * @reboot_required true
 * @min 0
 * @max 782097
 * @category Developer
 * @group Circuit Breaker
 */
PARAM_DEFINE_INT32(CBRK_BUZZER, 0);

/**
 * Circuit breaker for USB link check
 *
 * Setting this parameter to 197848 will disable the USB connected
 * checks in the commander, setting it to 0 keeps them enabled (recommended).
 *
 * We are generally recommending to not fly with the USB link
 * connected and production vehicles should set this parameter to
 * zero to prevent users from flying USB powered. However, for R&D purposes
 * it has proven over the years to work just fine.
 *
 * @min 0
 * @max 197848
 * @category Developer
 * @group Circuit Breaker
 */
PARAM_DEFINE_INT32(CBRK_USB_CHK, 197848);

/**
 * Circuit breaker for arming in fixed-wing mode check
 *
 * Setting this parameter to 159753 will enable arming in fixed-wing
 * mode for VTOLs.
 * WARNING: ENABLING THIS CIRCUIT BREAKER IS AT OWN RISK
 *
 * @min 0
 * @max 159753
 * @category Developer
 * @group Circuit Breaker
 */
PARAM_DEFINE_INT32(CBRK_VTOLARMING, 0);
