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
#include <semaphore.h>
#include <px4_platform_common/workqueue.h>

#pragma once

__BEGIN_DECLS

extern px4_sem_t       _hrt_work_lock;
extern struct wqueue_s g_hrt_work;

void hrt_work_queue_init(void);
int  hrt_work_queue(struct work_s *work, worker_t worker, void *arg, uint32_t usdelay);
void hrt_work_cancel(struct work_s *work);

static inline void hrt_work_lock(void);

static inline void hrt_work_lock() {
    px4_sem_wait(&_hrt_work_lock);
}

static inline void hrt_work_unlock(void);

static inline void hrt_work_unlock() {
    px4_sem_post(&_hrt_work_lock);
}

__END_DECLS
