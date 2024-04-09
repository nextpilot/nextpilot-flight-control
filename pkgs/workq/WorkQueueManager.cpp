/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "WorkQueueManager"

#include <rtthread.h>
#include <rtdbg.h>
#include <WorkQueueManager.hpp>
#include <WorkQueue.hpp>
#include <hrtimer.h>
#include <atomic.hpp>
#include <BlockingIntrusiveSortedList.hpp>
#include <BlockingRingBuffer.hpp>
#include <limits.h>
#include <string.h>

using namespace time_literals;

namespace nextpilot {

// list of current work queues
static BlockingList<WorkQueue *> *_wq_manager_wqs_list{nullptr};

// queue of WorkQueues to be created (as threads in the wq manager task)
static BlockingQueue<const wq_config_t *, 1> *_wq_manager_create_queue{nullptr};

static atomic_bool _wq_manager_should_exit{true};

static WorkQueue *FindWorkQueueByName(const char *name) {
    if (_wq_manager_wqs_list == nullptr) {
        LOG_E("not running");
        return nullptr;
    }

    LockGuard lg{_wq_manager_wqs_list->mutex()};

    // search list
    for (WorkQueue *wq : *_wq_manager_wqs_list) {
        if (strcmp(wq->get_name(), name) == 0) {
            return wq;
        }
    }

    return nullptr;
}

WorkQueue *WorkQueueFindOrCreate(const wq_config_t &new_wq) {
    if (_wq_manager_create_queue == nullptr) {
        LOG_E("not running");
        return nullptr;
    }

    // search list for existing work queue
    WorkQueue *wq = FindWorkQueueByName(new_wq.name);

    // create work queue if it doesn't already exist
    if (wq == nullptr) {
        // add WQ config to list
        //  main thread wakes up, creates the thread
        _wq_manager_create_queue->push(&new_wq);

        // we wait until new wq is created, then return
        uint64_t t = 0;

        while (wq == nullptr && t < 10_s) {
            // Wait up to 10 seconds, checking every 1 ms
            t += 1_ms;
            usleep(1_ms);

            wq = FindWorkQueueByName(new_wq.name);
        }

        if (wq == nullptr) {
            LOG_E("failed to create %s", new_wq.name);
        }
    }

    return wq;
}

const wq_config_t &device_bus_to_wq(uint32_t device_id_int) {
    // union device::Device::DeviceId device_id;
    // device_id.devid = device_id_int;

    // const device::Device::DeviceBusType bus_type = device_id.devid_s.bus_type;
    // const uint8_t                       bus      = device_id.devid_s.bus;

    // if (bus_type == device::Device::DeviceBusType_I2C) {
    //     switch (bus) {
    //     case 0: return wq_configurations::I2C0;

    //     case 1: return wq_configurations::I2C1;

    //     case 2: return wq_configurations::I2C2;

    //     case 3: return wq_configurations::I2C3;

    //     case 4: return wq_configurations::I2C4;
    //     }

    // } else if (bus_type == device::Device::DeviceBusType_SPI) {
    //     switch (bus) {
    //     case 0: return wq_configurations::SPI0;

    //     case 1: return wq_configurations::SPI1;

    //     case 2: return wq_configurations::SPI2;

    //     case 3: return wq_configurations::SPI3;

    //     case 4: return wq_configurations::SPI4;

    //     case 5: return wq_configurations::SPI5;

    //     case 6: return wq_configurations::SPI6;
    //     }
    // }

    // otherwise use high priority
    return wq_configurations::hp_default;
};

const wq_config_t &serial_port_to_wq(const char *serial) {
    if (serial == nullptr) {
        return wq_configurations::ttyUnknown;

    } else if (strstr(serial, "ttyS0")) {
        return wq_configurations::ttyS0;

    } else if (strstr(serial, "ttyS1")) {
        return wq_configurations::ttyS1;

    } else if (strstr(serial, "ttyS2")) {
        return wq_configurations::ttyS2;

    } else if (strstr(serial, "ttyS3")) {
        return wq_configurations::ttyS3;

    } else if (strstr(serial, "ttyS4")) {
        return wq_configurations::ttyS4;

    } else if (strstr(serial, "ttyS5")) {
        return wq_configurations::ttyS5;

    } else if (strstr(serial, "ttyS6")) {
        return wq_configurations::ttyS6;

    } else if (strstr(serial, "ttyS7")) {
        return wq_configurations::ttyS7;

    } else if (strstr(serial, "ttyS8")) {
        return wq_configurations::ttyS8;

    } else if (strstr(serial, "ttyS9")) {
        return wq_configurations::ttyS9;

    } else if (strstr(serial, "ttyACM0")) {
        return wq_configurations::ttyACM0;
    }

    LOG_D("unknown serial port: %s", serial);

    return wq_configurations::ttyUnknown;
}

const wq_config_t &ins_instance_to_wq(uint8_t instance) {
    switch (instance) {
    case 0: return wq_configurations::INS0;

    case 1: return wq_configurations::INS1;

    case 2: return wq_configurations::INS2;

    case 3: return wq_configurations::INS3;
    }

    LOG_W("no INS%d wq configuration, using INS0", instance);

    return wq_configurations::INS0;
}

static void WorkQueueRunnerEntry(void *param) {
    wq_config_t *config = static_cast<wq_config_t *>(param);

    // 创建workqueue
    WorkQueue wq(*config);

    // add to work queue list
    _wq_manager_wqs_list->add(&wq);

    wq.Run();

    // remove from work queue list
    _wq_manager_wqs_list->remove(&wq);
}

static void WorkQueueManagerEntry(void *param) {
    while (!_wq_manager_should_exit.load()) {
        // create new work queues as needed
        const wq_config_t *wq = _wq_manager_create_queue->pop();

        if (wq != nullptr) {
            // create new work queue

            // stack size
#if defined(__PX4_NUTTX) || defined(__PX4_QURT)
            const size_t stacksize = math::max(PTHREAD_STACK_MIN, PX4_STACK_ADJUSTED(wq->stacksize));
#elif defined(__PX4_POSIX)
            // On posix system , the desired stacksize round to the nearest multiplier of the system pagesize
            // It is a requirement of the  pthread_attr_setstacksize* function
            const unsigned int page_size     = sysconf(_SC_PAGESIZE);
            const size_t       stacksize_adj = math::max((int)PTHREAD_STACK_MIN, PX4_STACK_ADJUSTED(wq->stacksize));
            const size_t       stacksize     = (stacksize_adj + page_size - (stacksize_adj % page_size));
#else
            const size_t stacksize = 2048;
#endif

            // priority
            // int sched_priority = sched_get_priority_max(SCHED_FIFO) + wq->relative_priority;
            int sched_priority = 10;

            // create workqueue runner thread

            rt_thread_t tid = rt_thread_create(wq->name,
                                               WorkQueueRunnerEntry,
                                               (void *)wq,
                                               stacksize,
                                               sched_priority,
                                               5);

            if (tid) {
                rt_thread_startup(tid);
                LOG_I("starting: %s, priority: %d, stack: %zu bytes", wq->name, sched_priority, stacksize);

            } else {
                LOG_E("fail to create thread for %s", wq->name);
            }
        }
    }
}

int WorkQueueManagerStart() {
    if (_wq_manager_should_exit.load() && (_wq_manager_create_queue == nullptr)) {
        _wq_manager_wqs_list     = new BlockingList<WorkQueue *>();
        _wq_manager_create_queue = new BlockingQueue<const wq_config_t *, 1>();

        if (!_wq_manager_wqs_list || !_wq_manager_create_queue) {
            LOG_E("create wqs_list and wqs_queue fail");
            return -1;
        }

        rt_thread_t tid = rt_thread_create("wq:manager", WorkQueueManagerEntry, nullptr, 1024, 10, 5);

        if (!tid) {
            LOG_E("create wq:manager thread fail");
            return -1;
        }

        if (rt_thread_startup(tid) != 0) {
            LOG_E("startup wq:manager thread fail");
            return -1;
        }

        _wq_manager_should_exit.store(false);

        LOG_I("init ok");
        return RT_EOK;
    } else {
        LOG_W("already running");
        return -1;
    }
}

int WorkQueueManagerStop() {
    if (!_wq_manager_should_exit.load()) {
        // error can't shutdown until all WorkItems are removed/stopped
        if ((_wq_manager_wqs_list != nullptr) && (_wq_manager_wqs_list->size() > 0)) {
            LOG_E("can't shutdown with active WQs");
            WorkQueueManagerStatus();
            return RT_ERROR;
        }

        // first ask all WQs to stop
        if (_wq_manager_wqs_list != nullptr) {
            {
                LockGuard lg{_wq_manager_wqs_list->mutex()};

                // ask all work queues (threads) to stop
                // NOTE: not currently safe without all WorkItems stopping first
                for (WorkQueue *wq : *_wq_manager_wqs_list) {
                    wq->request_stop();
                }
            }

            // wait until they're all stopped (empty list)
            while (_wq_manager_wqs_list->size() > 0) {
                usleep(1000);
            }

            delete _wq_manager_wqs_list;
        }

        _wq_manager_should_exit.store(true);

        if (_wq_manager_create_queue != nullptr) {
            // push nullptr to wake the wq manager task
            _wq_manager_create_queue->push(nullptr);

            usleep(10000);

            delete _wq_manager_create_queue;
        }

    } else {
        LOG_W("not running");
        return RT_ERROR;
    }

    return RT_EOK;
}

int WorkQueueManagerStatus() {
    if (!_wq_manager_should_exit.load() && (_wq_manager_wqs_list != nullptr)) {
        const size_t num_wqs = _wq_manager_wqs_list->size();
        LOG_RAW("\nWork Queue: %-2zu threads                          RATE        INTERVAL\n", num_wqs);

        LockGuard lg{_wq_manager_wqs_list->mutex()};
        size_t    i = 0;

        for (WorkQueue *wq : *_wq_manager_wqs_list) {
            i++;

            const bool last_wq = (i >= num_wqs);

            if (!last_wq) {
                LOG_RAW("|__ %zu) ", i);

            } else {
                LOG_RAW("\\__ %zu) ", i);
            }

            wq->print_status(last_wq);
        }

    } else {
        LOG_I("not running");
    }

    return RT_EOK;
}

} // namespace nextpilot

#include <module_usage.h>
static void usage() {
    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description

Command-line tool to show work queue status.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("workq", "system");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

static int work_queue_main(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
        return 1;
    }

    if (!strcmp(argv[1], "start")) {
        nextpilot::WorkQueueManagerStart();
        return 0;

    } else if (!strcmp(argv[1], "stop")) {
        nextpilot::WorkQueueManagerStop();
        return 0;

    } else if (!strcmp(argv[1], "status")) {
        nextpilot::WorkQueueManagerStatus();
        return 0;
    }

    usage();

    return 0;
}
MSH_CMD_EXPORT_ALIAS(work_queue_main, workq, work queue manager);

static int work_queue_status() {
    nextpilot::WorkQueueManagerStatus();
    return 0;
}
MSH_CMD_EXPORT_ALIAS(work_queue_status, list_wq, list work manager);

static int work_queue_start() {
    nextpilot::WorkQueueManagerStart();
    return 0;
}
// 这里需要注意，C++全局变量是在INIT_COMPONENT_EXPORT阶段初始化的
INIT_EXPORT(work_queue_start, "5.0");
