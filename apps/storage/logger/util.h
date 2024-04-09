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

#include <stdint.h>
#include <time.h>

#include <uORB/uORB.h>

#ifdef __PX4_NUTTX
#define LOG_DIR_LEN 64
#else
#define LOG_DIR_LEN 256
#endif

namespace px4 {
namespace logger {
namespace util {

/**
 * Recursively remove a directory
 * @return 0 on success, <0 otherwise
 */
int remove_directory(const char *dir);

/**
 * check if a file exists
 */
bool file_exist(const char *filename);

/**
 * Check if there is enough free space left on the SD Card.
 * It will remove old log files if there is not enough space,
 * and if that fails return 1, and send a user message
 * @param log_root_dir log root directory: it's expected to contain directories in the form of sess%i or %d-%d-%d (year, month, day)
 * @param max_log_dirs_to_keep maximum log directories to keep (set to 0 for unlimited)
 * @param mavlink_log_pub
 * @param sess_dir_index output argument: will be set to the next free directory sess%i index.
 * @return 0 on success, 1 if not enough space, <0 on error
 */
int check_free_space(const char *log_root_dir, int32_t max_log_dirs_to_keep, orb_advert_t &mavlink_log_pub,
                     int &sess_dir_index);

/**
 * Get the time for log file name
 * @param tt returned time
 * @param utc_offset_sec UTC time offset [s]
 * @param boot_time use time when booted instead of current time
 * @return true on success, false otherwise (eg. if no gps)
 */
bool get_log_time(struct tm *tt, int utc_offset_sec = 0, bool boot_time = false);

}
}
} // namespace px4::logger::util
