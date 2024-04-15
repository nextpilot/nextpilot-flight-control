/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file printload.h
 *
 * Print the current system load.
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#pragma once

#include "cpuload.h"
#include <stdint.h>

struct print_load_s {
    uint64_t total_user_time{0};

    int running_count{0};
    int blocked_count{0};

    uint64_t new_time{0};
    uint64_t interval_start_time{0};
    uint64_t last_times[CONFIG_FS_PROCFS_MAX_TASKS]{};
    float    interval_time_us{0.f};
};

__BEGIN_DECLS

__EXPORT void init_print_load(struct print_load_s *s);

__EXPORT void print_load(int fd, struct print_load_s *print_state);

typedef void (*print_load_callback_f)(void *user);

/**
 * Print load to a buffer, and call cb after each written line (buffer will not include '\n')
 */
void print_load_buffer(char *buffer, int buffer_length, print_load_callback_f cb, void *user,
                       struct print_load_s *print_state);

__END_DECLS
