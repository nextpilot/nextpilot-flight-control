/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#ifndef __NEXTPILOT_TASK_H__
#define __NEXTPILOT_TASK_H__

#include <stdbool.h>
#include "rtconfig.h"

#define SCHED_PRIORITY_MAX     RT_THREAD_PRIORITY_MAX
#define SCHED_PRIORITY_MIN     0
#define SCHED_PRIORITY_DEFAULT 20

// work queue starting high priority
#define WQ_HP_BASE 10

// Fast drivers - they need to run as quickly as possible to minimize control
// latency.
#define SCHED_PRIORITY_FAST_DRIVER (WQ_HP_BASE - 1) // 9

// Actuator outputs should run as soon as the rate controller publishes
// the actuator controls topic
#define SCHED_PRIORITY_ACTUATOR_OUTPUTS (WQ_HP_BASE + 3) // 13

// Attitude controllers typically are in a blocking wait on driver data
// they should be the first to run on an update, using the current sensor
// data and the *previous* attitude reference from the position controller
// which typically runs at a slower rate
#define SCHED_PRIORITY_ATTITUDE_CONTROL (WQ_HP_BASE + 4) // 14

// Estimators should run after the attitude controller but before anything
// else in the system. They wait on sensor data which is either coming
// from the sensor hub or from a driver. Keeping this class at a higher
// priority ensures that the estimator runs first if it can, but will
// wait for the sensor hub if its data is coming from it.
#define SCHED_PRIORITY_ESTIMATOR (WQ_HP_BASE + 5) // 15

// Logger watchdog priority, triggered when a task busy-loops (and
// restored after a short time).
// The priority is a trade-off between:
// - ability to capture any busy-looping task below this priority
// - not having a negative impact on the system itself
#define SCHED_PRIORITY_LOG_WATCHDOG (WQ_HP_BASE + 6) // 16

// Position controllers typically are in a blocking wait on estimator data
// so when new sensor data is available they will run last. Keeping them
// on a high priority ensures that they are the first process to be run
// when the estimator updates.
#define SCHED_PRIORITY_POSITION_CONTROL (WQ_HP_BASE + 7) // 17

// The log capture (which stores log data into RAM) should run faster
// than other components, but should not run before the control pipeline
#define SCHED_PRIORITY_LOG_CAPTURE (WQ_HP_BASE + 10)

// Slow drivers should run at a rate where they do not impact the overall
// system execution
#define SCHED_PRIORITY_SLOW_DRIVER (WQ_HP_BASE + 11) // 21

// The navigation system needs to execute regularly but has no realtime needs
#define SCHED_PRIORITY_NAVIGATION (WQ_HP_BASE + 7) // 17

//      SCHED_PRIORITY_DEFAULT
#define SCHED_PRIORITY_LOG_WRITER (WQ_HP_BASE + 11) // 21
#define SCHED_PRIORITY_PARAMS     (WQ_HP_BASE + 12) // 22
//      SCHED_PRIORITY_IDLE

#endif // __NEXTPILOT_TASK_H__
