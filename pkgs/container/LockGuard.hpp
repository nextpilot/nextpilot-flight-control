/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <pthread.h>
class LockGuard {
public:
    explicit LockGuard(pthread_mutex_t &mutex) :
        _mutex(mutex) {
        pthread_mutex_lock(&_mutex);
    }

    LockGuard(const LockGuard &other)            = delete;
    LockGuard &operator=(const LockGuard &other) = delete;

    ~LockGuard() {
        pthread_mutex_unlock(&_mutex);
    }

private:
    pthread_mutex_t &_mutex;
};
