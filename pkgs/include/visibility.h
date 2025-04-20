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

#ifndef __EXPORT
#   if __GNUC__ >= 4
#      define __EXPORT __attribute__((visibility("default")))
#   else
#      define __EXPORT
#   endif
#endif // __EXPORT

#ifndef __PRIVATE
#   if __GNUC__ >= 4
#      define __PRIVATE __attribute__((visibility("hidden")))
#   else
#      define __PRIVATE
#   endif
#endif //__PRIVATE

#ifdef __cplusplus
#   define __BEGIN_DECLS extern "C" {
#   define __END_DECLS   }
#else
#   define __BEGIN_DECLS
#   define __END_DECLS
#endif

#endif // __NEXTPILOT_VISIBILITY_H__
