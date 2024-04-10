/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/// @file calibration_routines.h
///	@author Don Gagne <don@thegagnes.com>

#pragma once

#include <hrtimer.h>
#include <uORB/uORBPublication.hpp>

// The order of these cannot change since the calibration calculations depend on them in this order
enum detect_orientation_return {
    ORIENTATION_TAIL_DOWN,
    ORIENTATION_NOSE_DOWN,
    ORIENTATION_LEFT,
    ORIENTATION_RIGHT,
    ORIENTATION_UPSIDE_DOWN,
    ORIENTATION_RIGHTSIDE_UP,
    ORIENTATION_ERROR
};
static constexpr unsigned detect_orientation_side_count = 6;

/// Wait for vehicle to become still and detect it's orientation
///	@return Returns detect_orientation_return according to orientation when vehicle
///		and ready for measurements
enum detect_orientation_return detect_orientation(orb_advert_t *mavlink_log_pub, ///< uORB handle to write output to
                                                  bool          lenient_still_detection); ///< true: Use more lenient still position detection

/// Returns the human readable string representation of the orientation
///	@param orientation Orientation to return string for, "error" if buffer is too small
const char *detect_orientation_str(enum detect_orientation_return orientation);

enum calibrate_return {
    calibrate_return_ok,
    calibrate_return_error,
    calibrate_return_cancelled
};

typedef calibrate_return (*calibration_from_orientation_worker_t)(detect_orientation_return
                                                                        orientation,  ///< Orientation which was detected
                                                                  void *worker_data); ///< Opaque worker data

/// Perform calibration sequence which require a rest orientation detection prior to calibration.
///	@return OK: Calibration succeeded, ERROR: Calibration failed
calibrate_return calibrate_from_orientation(orb_advert_t                         *mavlink_log_pub,                                    ///< uORB handle to write output to
                                            bool                                  side_data_collected[detect_orientation_side_count], ///< Sides for which data still needs calibration
                                            calibration_from_orientation_worker_t calibration_worker,                                 ///< Worker routine which performs the actual calibration
                                            void                                 *worker_data,                                        ///< Opaque data passed to worker routine
                                            bool                                  lenient_still_detection);                                                            ///< true: Use more lenient still position detection

/// Used to periodically check for a cancel command
bool calibrate_cancel_check(orb_advert_t *mavlink_log_pub, const hrt_abstime &calibration_started);

// TODO FIXME: below are workarounds for QGC. The issue is that sometimes
// a mavlink log message is overwritten by the following one. A workaround
// is to wait for some time after publishing each message and hope that it
// gets sent out in the meantime.

#define calibration_log_info(_pub, _text, ...)        \
    do {                                              \
        mavlink_log_info(_pub, _text, ##__VA_ARGS__); \
        usleep(10000);                                \
    } while (0);

#define calibration_log_critical(_pub, _text, ...)        \
    do {                                                  \
        mavlink_log_critical(_pub, _text, ##__VA_ARGS__); \
        usleep(10000);                                    \
    } while (0);

#define calibration_log_emergency(_pub, _text, ...)        \
    do {                                                   \
        mavlink_log_emergency(_pub, _text, ##__VA_ARGS__); \
        usleep(10000);                                     \
    } while (0);
