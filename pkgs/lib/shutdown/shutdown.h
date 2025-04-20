/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __SHUTDOWN_H__
#define __SHUTDOWN_H__

/**
 * @file shutdown.h
 * Power-related API
 */

#pragma once

#include <stdbool.h>
#include <inttypes.h>
#include <defines.h>

__BEGIN_DECLS

/**
 * Shutdown hook callback method (@see register_shutdown_hook()).
 * @return true if it's ok to shutdown, false if more time needed for cleanup
 */
typedef bool (*shutdown_hook_t)(void);

/**
 * Register a method that should be called when powering off (and also on reboot).
 * @param hook callback method. It must not block, but return immediately.
 *        When the system is requested to shutdown, the registered hooks will be
 *        called regularily until either all of them return true, or a timeout
 *        is reached.
 * @return 0 on success, <0 on error
 */
__EXPORT int register_shutdown_hook(shutdown_hook_t hook);

/**
 * Unregister a shutdown hook
 * @param hook callback method to be removed
 * @return 0 on success, <0 on error
 */
__EXPORT int unregister_shutdown_hook(shutdown_hook_t hook);

/**
 * Request the system to reboot.
 * Note the following:
 * - The system might not support reboot. In that case -EINVAL will
 *   be returned.
 * - The system might not shutdown immediately, so expect this method to return even
 *   on success.
 * @param to_bootloader reboot into bootloader mode (only used if reboot is true)
 * @param delay_us optional delay in microseconds
 * @return 0 on success, <0 on error
 */
#if defined(CONFIG_BOARDCTL_RESET)
__EXPORT int reboot_request(bool to_bootloader = false, uint32_t delay_us = 0);
#endif // CONFIG_BOARDCTL_RESET

/**
 * Request the system to shut down or reboot.
 * Note the following:
 * - The system might not support shutdown. In that case -EINVAL will
 *   be returned.
 * - The system might not shutdown immediately, so expect this method to return even
 *   on success.
 * @param delay_us optional delay in microseconds
 * @return 0 on success, <0 on error
 */
#if defined(BOARD_HAS_POWER_CONTROL) || defined(__PX4_POSIX)
__EXPORT int shutdown_request(uint32_t delay_us = 0);
#endif // BOARD_HAS_POWER_CONTROL

/**
 * Grab the shutdown lock. It will prevent the system from shutting down until the lock is released.
 * It is safe to call this recursively.
 * @return 0 on success, <0 on error
 */
__EXPORT int shutdown_lock(void);

/**
 * Release the shutdown lock.
 * @return 0 on success, <0 on error
 */
__EXPORT int shutdown_unlock(void);

__END_DECLS

#endif // __SHUTDOWN_H__
