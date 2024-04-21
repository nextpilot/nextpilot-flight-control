/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
#pragma once

#ifndef CONFIG_FS_PROCFS_MAX_TASKS
#   define CONFIG_FS_PROCFS_MAX_TASKS 64
#endif

// #ifdef CONFIG_SCHED_INSTRUMENTATION

#include <defines.h>
#include <stdint.h>
#include <stdbool.h>

struct system_load_taskinfo_s {
    uint64_t      total_runtime{0};   ///< Runtime since start (start_time - total_runtime)/(start_time - current_time) = load
    uint64_t      curr_start_time{0}; ///< Start time of the current scheduling slot
    struct tcb_s *tcb{nullptr};
    bool          valid{false};       ///< Task is currently active / valid
};

struct system_load_s {
    uint64_t               start_time{0};
    system_load_taskinfo_s tasks[CONFIG_FS_PROCFS_MAX_TASKS]{};
    int                    total_count{0};
    int                    running_count{0};
    bool                   initialized{false};
};

__BEGIN_DECLS

__EXPORT extern struct system_load_s system_load;

__EXPORT void cpuload_initialize_once(void);

__EXPORT void cpuload_monitor_start(void);
__EXPORT void cpuload_monitor_stop(void);

__END_DECLS

// #endif
