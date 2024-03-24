/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#ifndef __MACROS_H__
#define __MACROS_H__

#if !defined(arraySize)
#define arraySize(a) (sizeof((a)) / sizeof((a[0])))
#endif

#if !defined(CAT)
#if !defined(_CAT)
#define _CAT(a, b) a##b
#endif
#define CAT(a, b) _CAT(a, b)
#endif

#if !defined(FREEZE_STR)
#define FREEZE_STR(s) #s
#endif

#if !defined(STRINGIFY)
#define STRINGIFY(s) FREEZE_STR(s)
#endif

// #if !defined(UNUSED)
// #define UNUSED(var) (void)(var)
// #endif

#endif // __MACROS_H__