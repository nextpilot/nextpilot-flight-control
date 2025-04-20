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
 * @file wqueue_example.cpp
 * Example for Linux
 *
 * @author Mark Charlebois <charlebm@gmail.com>
 */

#include "wqueue_test.h"

#include <px4_platform_common/time.h>
#include <px4_platform_common/workqueue.h>
#include <unistd.h>
#include <stdio.h>

px4::AppState WQueueTest::appState;

void WQueueTest::hp_worker_cb(void *p) {
    WQueueTest *wqep = (WQueueTest *)p;

    wqep->do_hp_work();
}

void WQueueTest::lp_worker_cb(void *p) {
    WQueueTest *wqep = (WQueueTest *)p;

    wqep->do_lp_work();
}

void WQueueTest::do_lp_work() {
    static int iter = 0;
    printf("done lp work\n");

    if (iter > 5) {
        _lpwork_done = true;
    }

    ++iter;

    work_queue(LPWORK, &_lpwork, (worker_t)&lp_worker_cb, this, 1000);
}

void WQueueTest::do_hp_work() {
    static int iter = 0;
    printf("done hp work\n");

    if (iter > 5) {
        _hpwork_done = true;
    }

    ++iter;

    // requeue
    work_queue(HPWORK, &_hpwork, (worker_t)&hp_worker_cb, this, 1000);
}

int WQueueTest::main() {
    appState.setRunning(true);

    //Put work on HP work queue
    work_queue(HPWORK, &_hpwork, (worker_t)&hp_worker_cb, this, 1000);


    //Put work on LP work queue
    work_queue(LPWORK, &_lpwork, (worker_t)&lp_worker_cb, this, 1000);


    // Wait for work to finsh
    while (!appState.exitRequested() && !(_hpwork_done && _lpwork_done)) {
        printf("  Sleeping for 2 sec...\n");
        sleep(2);
    }

    return 0;
}
