/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file BlockingList.hpp
 *
 * A blocking intrusive linked list.
 */

#pragma once

#include "IntrusiveSortedList.hpp"
#include "LockGuard.hpp"

#include <pthread.h>
#include <stdlib.h>

template <class T>
class BlockingList : public IntrusiveSortedList<T> {
public:
    ~BlockingList() {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cv);
    }

    void add(T newNode) {
        LockGuard lg{_mutex};
        IntrusiveSortedList<T>::add(newNode);
    }

    bool remove(T removeNode) {
        LockGuard lg{_mutex};
        return IntrusiveSortedList<T>::remove(removeNode);
    }

    size_t size() {
        LockGuard lg{_mutex};
        return IntrusiveSortedList<T>::size();
    }

    void clear() {
        LockGuard lg{_mutex};
        IntrusiveSortedList<T>::clear();
    }

    pthread_mutex_t &mutex() {
        return _mutex;
    }

private:
    pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  _cv    = PTHREAD_COND_INITIALIZER;
};
