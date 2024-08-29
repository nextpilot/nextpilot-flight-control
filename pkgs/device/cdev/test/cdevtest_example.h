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
 * @file vcdevtest_example.h
 * Example app for Linux
 *
 * @author Mark Charlebois <charlebm@gmail.com>
 */
#pragma once

#include <px4_platform_common/app.h>

class CDevNode;

class CDevExample {
public:
    CDevExample() :
        _node(0) {
    }

    ~CDevExample();

    int main();

    static px4::AppState appState; /* track requests to terminate app */

private:
    int do_poll(int fd, int timeout, int iterations, int delayms_after_poll);

    CDevNode *_node;
};
