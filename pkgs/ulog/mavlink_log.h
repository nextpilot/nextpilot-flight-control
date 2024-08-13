/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __MAVLINK_LOG_H__
#define __MAVLINK_LOG_H__

#include <rtdbg.h>
#include <inttypes.h>
#ifdef PKG_USING_HRTIMER
#   include "hrtimer.h"
#endif // PKG_USING_HRTIMER

#ifdef PKG_USING_UORB
#   include "uORB.h"
#   include "topics/mavlink_log.h"

static inline void mavlink_vasprintf(uint8_t severity, orb_advert_t *mavlink_log_pub, const char *fmt, ...) {
    if (!fmt) {
        return;
    }

    if (!mavlink_log_pub) {
        return;
    }

    struct mavlink_log_s log_msg;
    log_msg.severity = severity;
#   ifdef PKG_USING_HRTIMER
    log_msg.timestamp = hrt_absolute_time();
#   else
    log_msg.timestamp = rt_tick_get() * 1000ULL;
#   endif // PKG_USING_HRTIMER

    va_list ap;
    va_start(ap, fmt);
    rt_vsnprintf((char *)log_msg.text, sizeof(log_msg.text), fmt, ap);
    va_end(ap);

    if (!(*mavlink_log_pub)) {
#   ifdef __cplusplus
        *mavlink_log_pub = orb_advertise_queue(ORB_ID(mavlink_log), &log_msg, mavlink_log_s::ORB_QUEUE_LENGTH);
#   else
        *mavlink_log_pub = orb_advertise_queue(ORB_ID(mavlink_log), &log_msg, MAVLINK_LOG_ORB_QUEUE_LENGTH);
#   endif //__cplusplus
    }

    if (*mavlink_log_pub != nullptr) {
        orb_publish(ORB_ID(mavlink_log), *mavlink_log_pub, &log_msg);
    }
}
#else
#   define mavlink_vasprintf(...)
#endif // PKG_USING_UORB

#define _MSG_PRIO_DEBUG     7
#define _MSG_PRIO_INFO      6
#define _MSG_PRIO_NOTICE    5
#define _MSG_PRIO_WARNING   4
#define _MSG_PRIO_ERROR     3
#define _MSG_PRIO_CRITICAL  2
#define _MSG_PRIO_ALERT     1
#define _MSG_PRIO_EMERGENCY 0

/**
 * Send a mavlink info message (also printed to console).
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_info(_pub, _text, ...)                             \
    do {                                                               \
        mavlink_vasprintf(_MSG_PRIO_INFO, _pub, _text, ##__VA_ARGS__); \
        LOG_I(_text, ##__VA_ARGS__);                                   \
    } while (0);

/**
 * Send a mavlink warning message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_warning(_pub, _text, ...)                             \
    do {                                                                  \
        mavlink_vasprintf(_MSG_PRIO_WARNING, _pub, _text, ##__VA_ARGS__); \
        LOG_W(_text, ##__VA_ARGS__);                                      \
    } while (0);

/**
 * Send a mavlink error message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_error(_pub, _text, ...)                             \
    do {                                                                \
        mavlink_vasprintf(_MSG_PRIO_ERROR, _pub, _text, ##__VA_ARGS__); \
        LOG_E(_text, ##__VA_ARGS__);                                    \
    } while (0);

/**
 * Send a mavlink critical message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_critical(_pub, _text, ...)                             \
    do {                                                                   \
        mavlink_vasprintf(_MSG_PRIO_CRITICAL, _pub, _text, ##__VA_ARGS__); \
        LOG_W(_text, ##__VA_ARGS__);                                       \
    } while (0);

/**
 * Send a mavlink emergency message and print to console.
 *
 * @param _pub		Pointer to the uORB advert;
 * @param _text		The text to log;
 */
#define mavlink_log_emergency(_pub, _text, ...)                             \
    do {                                                                    \
        mavlink_vasprintf(_MSG_PRIO_EMERGENCY, _pub, _text, ##__VA_ARGS__); \
        LOG_W(_text, ##__VA_ARGS__);                                        \
    } while (0);

#endif // __MAVLINK_LOG_H__
