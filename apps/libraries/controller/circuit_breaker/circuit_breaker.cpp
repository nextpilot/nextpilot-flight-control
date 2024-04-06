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
 * @file circuit_breaker.c
 *
 * Circuit breaker parameters.
 * Analog to real aviation circuit breakers these parameters
 * allow to disable subsystems. They are not supported as standard
 * operation procedure and are only provided for development purposes.
 * To ensure they are not activated accidentally, the associated
 * parameter needs to set to the key (magic).
 */

#include "circuit_breaker.h"

#include <stdint.h>
// #include <px4_platform_common/defines.h>
#include <param.h>

bool circuit_breaker_enabled(const char *breaker, int32_t magic) {
    int32_t val = -1;

    return (param_get(param_find(breaker), &val) == 0) && (val == magic);
}
