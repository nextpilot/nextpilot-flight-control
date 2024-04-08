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
#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
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
#include "queue.h"
#endif // PKG_USING_QUEUE

#ifdef PKG_USING_LIBCRC
#include "crc8.h"
#include "crc16.h"
#include "crc32.h"
#include "crc64.h"
#endif // PKG_USING_LIBCRC

// hrtimer
#ifdef PKG_USING_HRTIMER
#include "hrtimer.h"
#endif // PKG_USING_HRTIMER

// uorb
#ifdef PKG_USING_UORB
#include "uORB.h"
#ifdef __cplusplus
#include "uORBPublication.hpp"
#include "uORBSubscription.hpp"
#include "topics/uORBTopics.hpp"
#endif //__cplusplus
#endif // PKG_USING_UORB

// param
#ifdef PKG_USING_PARAM
#include "param.h"
#endif // PKG_USING_PARAM

// workq
#ifdef PKG_USING_WORKQ
#include "WorkItem.hpp"
#include "WorkItemScheduled.hpp"
#include "WorkItemSingleShot.hpp"
#endif // PKG_USING_WORKQ

#ifdef PKG_USING_CONTAINER
#include "container.h"
#endif // PKG_USING_CONTAINER

#ifdef PKG_USING_BOARD_VERSION
#include "board_version.h"
#endif // PKG_USING_BOARD_VERSION

// getopt
#ifdef PKG_USING_GETOPT
#include "getopt.h"
#endif // PKG_USING_GETOPT

// scope
#ifdef PKG_USING_SCOPE
#endif // PKG_USING_SCOPE

// module
#include "module_usage.h"
#if defined(__cplusplus)
#include "module_command.hpp"
#include "module_params.hpp"
#endif // PKG_USING_WORKQ

// mathlib
#if defined(PKG_USING_MATHLIB) && defined(__cplusplus)
#include "mathlib.h"
#include "math/filter/AlphaFilter.hpp"
#include "math/filter/LowPassFilter2p.hpp"
#include "math/filter/MedianFilter.hpp"
#include "math/filter/NotchFilter.hpp"
#include "math/filter/second_order_reference_model.hpp"
#endif // PKG_USING_MATHLIB

// matrix
#if defined(PKG_USING_MATRIX) && defined(cplusplus)
#include "matrix/math.hpp"
#endif // PKG_USING_MATRIX

#endif // __NEXTPILOT_H__
