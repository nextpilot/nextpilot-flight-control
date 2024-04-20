/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __NEXTPILOT_VISIBILITY_H__
#define __NEXTPILOT_VISIBILITY_H__

#ifdef __EXPORT
#   undef __EXPORT
#endif
#define __EXPORT __attribute__((visibility("default")))

#ifdef __PRIVATE
#   undef __PRIVATE
#endif
#define __PRIVATE __attribute__((visibility("hidden")))

#ifdef __cplusplus
#   define __BEGIN_DECLS extern "C" {
#   define __END_DECLS   }
#else
#   define __BEGIN_DECLS
#   define __END_DECLS
#endif

#endif // __NEXTPILOT_VISIBILITY_H__
