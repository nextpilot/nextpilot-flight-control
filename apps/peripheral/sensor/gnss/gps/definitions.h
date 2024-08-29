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
 * @file definitions.h
 * common platform-specific definitions & abstractions for gps
 * @author Beat Küng <beat-kueng@gmx.net>
 */

#pragma once

#include <drivers/drv_hrt.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/log.h>
#include <uORB/topics/satellite_info.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/sensor_gnss_relative.h>

#define GPS_INFO(...) PX4_INFO(__VA_ARGS__)
#define GPS_WARN(...) PX4_WARN(__VA_ARGS__)
#define GPS_ERR(...)  PX4_ERR(__VA_ARGS__)

#define gps_usleep    px4_usleep

/**
 * Get the current time in us. Function signature:
 * uint64_t hrt_absolute_time()
 */
#define gps_absolute_time hrt_absolute_time
typedef hrt_abstime gps_abstime;


// TODO: this functionality is not available on the Snapdragon yet
#ifdef __PX4_QURT
#   define NO_MKTIME
#endif
