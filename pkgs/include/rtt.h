/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __NEXTPIOT_RTT_H__
#define __NEXTPIOT_RTT_H__

/* board init routines will be called in board_init() function */
// #define INIT_BOARD_EXPORT(fn) INIT_EXPORT(fn, "1")
/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initialization) */
// #define INIT_PREV_EXPORT(fn) INIT_EXPORT(fn, "2")
/* device initialization */
// #define INIT_DEVICE_EXPORT(fn) INIT_EXPORT(fn, "3")
/* components initialization (dfs, lwip, ...) */
// #define INIT_COMPONENT_EXPORT(fn) INIT_EXPORT(fn, "4")
/* environment initialization (mount disk, ...) */
// #define INIT_ENV_EXPORT(fn) INIT_EXPORT(fn, "5")
/* application initialization (rtgui application etc ...) */
// #define INIT_APP_EXPORT(fn) INIT_EXPORT(fn, "6")

// #define INIT_BOARD_EXPORT_ORDER(fn, order)      INIT_EXPORT(fn, "0." #order)
// #define INIT_PREV_EXPORT_ORDER(fn, order)       INIT_EXPORT(fn, "1." #order)
// #define INIT_DEVICE_EXPORT_ORDER(fn, order)     INIT_EXPORT(fn, "2." #order)
// #define INIT_COMPONENT_EXPORT_ORDER(fn, odrder) INIT_EXPORT(fn, "3." #order)
// #define INIT_ENV_EXPORT_ORDER(fn, order)        INIT_EXPORT(fn, "4." #order)
// #define INIT_APP_EXPORT_ORDER(fn, order)        INIT_EXPORT(fn, "5." #order)

#define INIT_EXPORT_UORB(fn)        INIT_EXPORT(fn, "5.0.1")
#define INIT_EXPORT_PARAM(fn)       INIT_EXPORT(fn, "5.0.2")
#define INIT_EXPORT_BOARD_PARAM(fn) INIT_EXPORT(fn, "5.0.3")
#define INIT_EXPORT_AIRFRAME(fn)    INIT_EXPORT(fn, "5.0.4")
#define INIT_EXPORT_WORKQ(fn)       INIT_EXPORT(fn, "5.0.5")
#define INIT_EXPORT_TONE(fn)        INIT_EXPORT(fn, "5.0.6")

#define INIT_EXPORT_DRIVER(fn)      INIT_EXPORT(fn, "5.4")
#define INIT_EXPORT_SYSTEM(fn)      INIT_EXPORT(fn, "5.5")
#define INIT_EXPORT_ESTIMATOR(fn)   INIT_EXPORT(fn, "5.6")
#define INIT_EXPORT_CONTROLLER(fn)  INIT_EXPORT(fn, "5.7")
#define INIT_EXPORT_SIMULATION(fn)  INIT_APP_EXPORT(fn)

/* 任务周期 */
#define SIMULATION_PERIOD_MS (5)
#define ATTITUDE_PERIOD_MS   (5)
#define POSITION_PERIOD_MS   (10)
#define SIMULATION_PERIOD_US (SIMULATION_PERIOD_MS * 1000U)
#define ATTITUDE_PERIOD_US   (ATTITUDE_PERIOD_MS * 1000U)
#define POSITION_PERIOD_US   (POSITION_PERIOD_MS * 1000U)


#endif // __NEXTPIOT_RTT_H__
