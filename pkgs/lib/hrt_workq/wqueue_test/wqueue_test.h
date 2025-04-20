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
 * @file wqueue_test.h
 * Example app for Linux
 *
 * @author Mark Charlebois <charlebm@gmail.com>
 */
#pragma once

#include <px4_platform_common/app.h>
#include <px4_platform_common/workqueue.h>
#include <string.h>

class WQueueTest {
public:
    WQueueTest()  = default;
    ~WQueueTest() = default;

    int main();

    static px4::AppState appState; /* track requests to terminate app */

private:
    static void hp_worker_cb(void *p);
    static void lp_worker_cb(void *p);

    void do_lp_work(void);
    void do_hp_work(void);

    bool   _lpwork_done{false};
    bool   _hpwork_done{false};
    work_s _lpwork{};
    work_s _hpwork{};
};
