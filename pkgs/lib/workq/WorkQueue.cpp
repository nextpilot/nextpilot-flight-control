/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "WorkQueue"

#include <rtthread.h>
#include <WorkQueue.hpp>
#include <WorkItem.hpp>
#include <string.h>
#include <hrtimer.h>

namespace nextpilot::workq {

WorkQueue::WorkQueue(const wq_config_t &config) :
    _config(config) {
#ifndef __RTTHREAD__
    rt_sem_init(&_qlock, "wq_qlock", 1, RT_IPC_FLAG_PRIO);
#endif

    rt_sem_init(&_process_lock, "wq_process_lock", 0, RT_IPC_FLAG_PRIO);

    rt_sem_init(&_exit_lock, "wq_exit_lock", 1, RT_IPC_FLAG_PRIO);
}

WorkQueue::~WorkQueue() {
    // 这时候lock是什么意思？？不是只是用来锁定_q的吗，但是这里没有操作_q呀
    // work_lock();

    // Synchronize with ::Detach
    rt_sem_take(&_exit_lock, RT_WAITING_FOREVER);
    rt_sem_detach(&_exit_lock);

    rt_sem_detach(&_process_lock);
    // work_unlock();

#ifndef __RTTHREAD__
    rt_sem_detach(&_qlock);
#endif
}

bool WorkQueue::Attach(WorkItem *item) {
    // 这时候lock是什么意思？？不是只是用来锁定_q的吗，但是这里没有操作_q呀
    // 且_work_item_list自带mutex_lock的
    // work_lock();

    if (!should_exit()) {
        _work_item_list.add(item);
        // work_unlock();
        return true;
    }

    // work_unlock();

    return false;
}

void WorkQueue::Detach(WorkItem *item) {
    bool exiting = false;
    // 这时候lock是什么意思？？不是只是用来锁定_q的吗，但是这里没有操作_q呀
    // work_lock();

    _work_item_list.remove(item);

    if (_work_item_list.size() == 0) {
        // shutdown, no active WorkItems
        LOG_D("stopping: %s, last active WorkItem closing", _config.name);

        // Deletion of this work queue might happen right after request_stop or
        // SignalWorkerThread. Use a separate lock to prevent premature deletion
        rt_sem_take(&_exit_lock, RT_WAITING_FOREVER);
        exiting = true;
        request_stop();
        SignalWorkerThread();
    }

    // work_unlock();

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
        _lockstep_component = lockstep_register_component();
    }

#endif // ENABLE_LOCKSTEP_SCHEDULER

    _q.push(item);
    work_unlock();

    SignalWorkerThread();
}

void WorkQueue::SignalWorkerThread() {
    // 当往执行列表中添加了新的item，激活工作队列线程

    int sem_val = _process_lock.value;

    if (sem_val <= 0) {
        rt_sem_release(&_process_lock);
    }
}

void WorkQueue::Cancel(WorkItem *item) {
    // 从执行队列里面取消item

    work_lock();
    _q.remove(item);
    work_unlock();
}

void WorkQueue::Clear() {
    // 清空执行队列

    work_lock();
    while (!_q.empty()) {
        _q.pop();
    }
    work_unlock();
}

void WorkQueue::Run() {
    while (!should_exit()) {
        // loop as the wait may be interrupted by a signal
        rt_sem_take(&_process_lock, RT_WAITING_FOREVER);

        work_lock(); // 只是保护_q在访问、增加、删除的时候不能被修改

        // process queued work
        while (!_q.empty()) {
            WorkItem *work = _q.pop();
            // 执行真正的任务的时候，需要先放开_q的锁定，否则Run如果有锁，可能会锁死了
            work_unlock(); // unlock work queue to run (item may requeue itself)
            work->RunPreamble();
            work->Run();
            // work->RunPostscript();
            // Note: after Run() we cannot access work anymore, as it might have been deleted
            work_lock(); // re-lock
        }

#if defined(ENABLE_LOCKSTEP_SCHEDULER)

        if (_q.empty()) {
            lockstep_unregister_component(_lockstep_component);
            _lockstep_component = -1;
        }

#endif // ENABLE_LOCKSTEP_SCHEDULER

        work_unlock();
    }

    LOG_D("%s: exiting", _config.name);
}

void WorkQueue::print_status(bool last) {
    const size_t num_items = _work_item_list.size();
    LOG_RAW("%-16s\n", get_name());
    unsigned i = 0;

    for (WorkItem *item : _work_item_list) {
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

} // namespace nextpilot::workq
