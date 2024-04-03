/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <rtthread.h>
#include <WorkQueue.hpp>
#include <WorkItem.hpp>
#include <string.h>
#include <hrtimer.h>

namespace nextpilot {

WorkQueue::WorkQueue(const wq_config_t &config) :
    _config(config) {
    // set the threads name
#ifdef __PX4_DARWIN
    pthread_setname_np(_config.name);
#elif defined(__RTTHREAD__)
    rt_thread_t tid = rt_thread_self();
    rt_memcpy(tid->parent.name, _config.name, RT_NAME_MAX);
#else
    pthread_setname_np(pthread_self(), _config.name);
#endif

#if !defined(__PX4_NUTTX) && !defined(__RTTHREAD__)
    rt_sem_init(&_qlock, "qlock", 1, RT_IPC_FLAG_PRIO);
#endif /* __PX4_NUTTX */

    rt_sem_init(&_process_lock, "process_lock", 0, RT_IPC_FLAG_PRIO);
    // px4_sem_setprotocol(&_process_lock, SEM_PRIO_NONE);

    rt_sem_init(&_exit_lock, "exit_lock", 1, RT_IPC_FLAG_PRIO);
    // px4_sem_setprotocol(&_exit_lock, SEM_PRIO_NONE);
}

WorkQueue::~WorkQueue() {
    work_lock();

    // Synchronize with ::Detach
    rt_sem_take(&_exit_lock, RT_WAITING_FOREVER);
    rt_sem_detach(&_exit_lock);

    rt_sem_detach(&_process_lock);
    work_unlock();

#if !defined(__PX4_NUTTX) && !defined(__RTTHREAD__)
    rt_sem_detach(&_qlock);
#endif /* __PX4_NUTTX */
}

bool WorkQueue::Attach(WorkItem *item) {
    work_lock();

    if (!should_exit()) {
        _work_items.add(item);
        work_unlock();
        return true;
    }

    work_unlock();

    return false;
}

void WorkQueue::Detach(WorkItem *item) {
    bool exiting = false;

    work_lock();

    _work_items.remove(item);

    if (_work_items.size() == 0) {
        // shutdown, no active WorkItems
        LOG_D("stopping: %s, last active WorkItem closing", _config.name);

        // Deletion of this work queue might happen right after request_stop or
        // SignalWorkerThread. Use a separate lock to prevent premature deletion
        rt_sem_take(&_exit_lock, RT_WAITING_FOREVER);
        exiting = true;
        request_stop();
        SignalWorkerThread();
    }

    work_unlock();

    // In case someone is deleting this wq already, signal
    // that it is now allowed
    if (exiting) {
        rt_sem_release(&_exit_lock);
    }
}

void WorkQueue::Submit(WorkItem *item) {
    work_lock();

#if defined(ENABLE_LOCKSTEP_SCHEDULER)

    if (_lockstep_component == -1) {
        _lockstep_component = px4_lockstep_register_component();
    }

#endif // ENABLE_LOCKSTEP_SCHEDULER

    _q.push(item);
    work_unlock();

    SignalWorkerThread();
}

void WorkQueue::SignalWorkerThread() {
    int sem_val = _process_lock.value;

    if (sem_val <= 0) {
        rt_sem_release(&_process_lock);
    }
}

void WorkQueue::Cancel(WorkItem *item) {
    work_lock();
    _q.remove(item);
    work_unlock();
}

void WorkQueue::Clear() {
    work_lock();
    while (!_q.empty()) {
        _q.pop();
    }
    work_unlock();
}

void WorkQueue::Run() {
    while (!should_exit()) {
        // loop as the wait may be interrupted by a signal
        do {
        } while (rt_sem_take(&_process_lock, RT_WAITING_FOREVER) != 0);

        work_lock();

        // process queued work
        while (!_q.empty()) {
            WorkItem *work = _q.pop();

            work_unlock(); // unlock work queue to run (item may requeue itself)
            work->RunPreamble();
            work->Run();
            // Note: after Run() we cannot access work anymore, as it might have been deleted
            work_lock(); // re-lock
        }

#if defined(ENABLE_LOCKSTEP_SCHEDULER)

        if (_q.empty()) {
            px4_lockstep_unregister_component(_lockstep_component);
            _lockstep_component = -1;
        }

#endif // ENABLE_LOCKSTEP_SCHEDULER

        work_unlock();
    }

    LOG_D("%s: exiting", _config.name);
}

void WorkQueue::print_status(bool last) {
    const size_t num_items = _work_items.size();
    LOG_RAW("%-16s\n", get_name());
    unsigned i = 0;

    for (WorkItem *item : _work_items) {
        i++;

        if (last) {
            LOG_RAW("    ");

        } else {
            LOG_RAW("|   ");
        }

        if (i < num_items) {
            LOG_RAW("|__%2d) ", i);

        } else {
            LOG_RAW("\\__%2d) ", i);
        }

        item->print_run_status();
    }
}

} // namespace nextpilot
