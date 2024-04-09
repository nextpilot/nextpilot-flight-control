/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "WorkItemSingleShot"

#include <WorkItemSingleShot.hpp>

namespace nextpilot {

WorkItemSingleShot::WorkItemSingleShot(const nextpilot::wq_config_t &config, worker_method method, void *argument) :
    nextpilot::WorkItem("<single_shot>", config), _argument(argument), _method(method) {
    rt_sem_init(&_sem, "wiss_lock", 0, RT_IPC_FLAG_PRIO);
}

WorkItemSingleShot::WorkItemSingleShot(const nextpilot::WorkItem &work_item, worker_method method, void *argument) :
    nextpilot::WorkItem("<single_shot>", work_item), _argument(argument), _method(method) {
    rt_sem_init(&_sem, "lock", 0, RT_IPC_FLAG_PRIO);
}

WorkItemSingleShot::~WorkItemSingleShot() {
    rt_sem_detach(&_sem);
}

void WorkItemSingleShot::wait() {
    while (rt_sem_take(&_sem, RT_WAITING_FOREVER) != 0) {}
}

void WorkItemSingleShot::Run() {
    _method(_argument);
    rt_sem_release(&_sem);
}

} // namespace nextpilot
