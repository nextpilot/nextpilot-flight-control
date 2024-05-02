/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>
#include <stdio.h>
#include "work_lock.h"

#include <px4_platform_common/sem.h>

extern px4_sem_t _work_lock[];

void work_lock(int id) {
    px4_sem_wait(&_work_lock[id]);
}

void work_unlock(int id) {
    px4_sem_post(&_work_lock[id]);
}
