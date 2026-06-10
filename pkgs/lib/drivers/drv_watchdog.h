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
 * @file drv_watchdog.h
 *
 *  watchdog driver interface.
 *
 */

#pragma once

#include <stdint.h>

__BEGIN_DECLS

void watchdog_init(void);
void watchdog_init_ex(int prescale, int reload); // Optional interface
void watchdog_pet(void);
__END_DECLS
