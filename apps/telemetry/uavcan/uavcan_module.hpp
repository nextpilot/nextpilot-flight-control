/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once


#include <drivers/device/device.h>

/**
 * @file uavcan.hpp
 *
 * Public header for the UAVCAN module
 *
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 * @author David Sidrane <david_s5@nscdg.com>
 * @author Holger Steinhaus <holger@steinhaus-home.de>
 */

// firmware paths
#define UAVCAN_MAX_PATH_LENGTH (128 + 40)
#define UAVCAN_SD_ROOT_PATH    "/fs/microsd/"
#define UAVCAN_FIRMWARE_PATH   UAVCAN_SD_ROOT_PATH "ufw"
#define UAVCAN_ROMFS_FW_PATH   "/etc/uavcan/fw"
#define UAVCAN_ROMFS_FW_PREFIX "_"

// logging
#define UAVCAN_NODE_DB_PATH UAVCAN_SD_ROOT_PATH "/uavcan.db"
#define UAVCAN_LOG_FILE     UAVCAN_NODE_DB_PATH "/trace.log"

// public prototypes
extern "C" __EXPORT int uavcan_main(int argc, char *argv[]);
