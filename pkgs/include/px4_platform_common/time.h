
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2025 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/types.h>


#define px4_clock_gettime          clock_getime
#define px4_clock_settime          clock_settime
#define px4_usleep                 usleep
#define px4_sleep                  sleep
#define px4_pthread_cond_timedwait pthread_cond_timedwait
