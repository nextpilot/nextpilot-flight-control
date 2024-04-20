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
 * @file calibration_messages.h
 *
 * Common calibration messages.
 *
 * @author Anton Babushkin <anton.babushkin@me.com>
 */

#ifndef CALIBRATION_MESSAGES_H_
#define CALIBRATION_MESSAGES_H_

// The calibration message defines which begin with CAL_QGC_ are used by QGroundControl to run a state
// machine to provide visual feedback for calibration. As such, the text for them or semantics of when
// they are displayed cannot be modified without causing QGC to break. If modifications are made, make
// sure to bump the calibration version number which will cause QGC to perform log based calibration
// instead of visual calibration until such a time as QGC is update to the new version.

// The number in the cal started message is used to indicate the version stamp for the current calibration code.
#define CAL_QGC_STARTED_MSG "[cal] calibration started: 2 %s"
#define CAL_QGC_DONE_MSG    "[cal] calibration done: %s"
#define CAL_QGC_FAILED_MSG  "[cal] calibration failed: %s"
// Warnings are deprecated because they were only used when it failed anyway.
// #define CAL_QGC_WARNING_MSG			"[cal] calibration warning: %s"
#define CAL_QGC_CANCELLED_MSG            "[cal] calibration cancelled"
#define CAL_QGC_PROGRESS_MSG             "[cal] progress <%u>"
#define CAL_QGC_ORIENTATION_DETECTED_MSG "[cal] %s orientation detected"
#define CAL_QGC_SIDE_DONE_MSG            "[cal] %s side done, rotate to a different side"

#define CAL_ERROR_SENSOR_MSG             "[cal] calibration failed: reading sensor"
#define CAL_ERROR_RESET_CAL_MSG          "[cal] calibration failed: to reset, sensor %u"
#define CAL_ERROR_READ_CAL_MSG           "[cal] calibration failed: to read calibration"
#define CAL_ERROR_APPLY_CAL_MSG          "[cal] calibration failed: to apply calibration"
#define CAL_ERROR_SET_PARAMS_MSG         "[cal] calibration failed: to set parameters"

#endif /* CALIBRATION_MESSAGES_H_ */
