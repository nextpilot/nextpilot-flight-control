/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include "LockGuard.hpp"
#include "rtthread.h"

template <class T, size_t N>
class BlockingQueue {
public:
    BlockingQueue() {
        rt_sem_init(&_sem_head, "rf_head", N, RT_IPC_FLAG_PRIO);
        rt_sem_init(&_sem_tail, "rf_tail", 0, RT_IPC_FLAG_PRIO);
    }

    ~BlockingQueue() {
        rt_sem_detach(&_sem_head);
        rt_sem_detach(&_sem_tail);
    }

    void push(T newItem) {
        rt_sem_take(&_sem_head, RT_WAITING_FOREVER);

        _data[_tail] = newItem;
        _tail        = (_tail + 1) % N;

        rt_sem_release(&_sem_tail);
    }

    T pop() {
        rt_sem_take(&_sem_tail, RT_WAITING_FOREVER);

        T ret = _data[_head];
        _head = (_head + 1) % N;

        rt_sem_release(&_sem_head);

        return ret;
    }

private:
    struct rt_semaphore _sem_head;
    struct rt_semaphore _sem_tail;

    T _data[N]{};

    size_t _head{0};
    size_t _tail{0};
};
