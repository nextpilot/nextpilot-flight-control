/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __NEXTPILOT_H__
#define __NEXTPILOT_H__

// c标准库
// #ifndef __cplusplus
// #include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
// #else
// #endif //__cplusplus

// rtthread头文件
#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>
#include <rthw.h>

// DFS
#ifdef RT_USING_DFS
#include <dfs_file.h>
#endif // RT_USING_DFS

// Posix Interface
#ifdef DFS_USING_POSIX
#include <unistd.h>
#endif // DFS_USING_POSIX

#ifdef RT_USING_PTHREADS
#include <pthread.h>
#endif // RT_USING_PTHREADS

//
#include "defines.h"
#include "mavlink_log.h"

// queue
#ifdef PKG_USING_QUEUE
#include "queue/queue.h"
#endif // PKG_USING_QUEUE

#ifdef PKG_USING_CONTAINER
#include "containers/container.h"
#endif // PKG_USING_CONTAINER

#ifdef PKG_USING_LIBCRC
#include "libcrc/crc8.h"
#include "libcrc/crc16.h"
#include "libcrc/crc32.h"
#include "libcrc/crc64.h"
#endif // PKG_USING_LIBCRC

// hrtimer
#ifdef PKG_USING_HRTIMER
#include "hrtimer.h"
#endif // PKG_USING_HRTIMER

// uorb
#ifdef PKG_USING_UORB
#include "uORB/uORB.h"
#ifdef __cplusplus
#include "uORB/Publication.hpp"
#include "uORB/Subscription.hpp"
#include "uORB/topics/uORBTopics.hpp"
#endif //__cplusplus
#endif // PKG_USING_UORB

// param
#ifdef PKG_USING_PARAM
#include "param/param.h"
#endif // PKG_USING_PARAM

// workq
#ifdef PKG_USING_WORKQUEUE
#include "workq/WorkItem.hpp"
#include "workq/WorkItemScheduled.hpp"
#include "workq/WorkItemSingleShot.hpp"
#endif // PKG_USING_WORKQUEUE

#ifdef PKG_USING_BOARD_VERSION
#include "board_version.h"
#endif // PKG_USING_BOARD_VERSION

// getopt
#ifdef PKG_USING_GETOPT
#include "getopt/getopt.h"
#endif // PKG_USING_GETOPT

// perf
#ifdef PKG_USING_PERF
#include "perf/perf_counter.h"
#endif // PKG_USING_PERF

// module
#include "module/module_usage.h"
#if defined(__cplusplus)
#include "module/module_command.hpp"
#include "module/module_params.hpp"
#include "module/module_thread.hpp"
using namespace nextpilot::global_params;
#endif //__cplusplus

// mathlib
#if defined(PKG_USING_MATHLIB) && defined(__cplusplus)
#include "mathlib/mathlib.h"
#include "mathlib/math/filter/AlphaFilter.hpp"
#include "mathlib/math/filter/LowPassFilter2p.hpp"
#include "mathlib/math/filter/MedianFilter.hpp"
#include "mathlib/math/filter/NotchFilter.hpp"
#include "mathlib/math/filter/second_order_reference_model.hpp"
#endif // PKG_USING_MATHLIB

// matrix
#if defined(PKG_USING_MATRIX) && defined(__cplusplus)
#include "matrix/math.hpp"
#endif // PKG_USING_MATRIX

// Circuit breaker
#ifdef PKG_USING_CIRCUIT_BREAKER
#include "circuit_breaker/circuit_breaker.h"
#endif // PKG_USING_CIRCUIT_BREAKER

#ifdef PKG_USING_HYSTERESIS
#include "hysteresis/hysteresis.h"
#endif // PKG_USING_HYSTERESIS

#ifdef PKG_USING_GEO
#include "geo/geo.h"
#endif // PKG_USING_GEO

#ifdef PKG_USING_EVENTS
#include "events/events.h"
#endif // PKG_USING_EVENTS

// scope
#ifdef PKG_USING_SCOPE
#endif // PKG_USING_SCOPE

#ifdef __cplusplus
using namespace nextpilot;
using namespace nextpilot::global_params;
#endif //__cplusplus

#endif // __NEXTPILOT_H__
