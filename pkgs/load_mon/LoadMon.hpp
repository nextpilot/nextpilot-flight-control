/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#if defined(__PX4_NUTTX)
#include <malloc.h>
#endif
#include <drivers/drv_hrt.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <px4_platform/cpuload.h>
#include <uORB/Publication.hpp>
#include <uORB/topics/cpuload.h>
#include <uORB/topics/task_stack_info.h>

#if defined(__PX4_LINUX)
#include <sys/times.h>
#endif

namespace load_mon {

class LoadMon : public ModuleBase<LoadMon>, public ModuleParams, public px4::ScheduledWorkItem {
public:
    LoadMon();
    ~LoadMon() override;

    static int task_spawn(int argc, char *argv[]);

    /** @see ModuleBase */
    static int custom_command(int argc, char *argv[]) {
        return print_usage("unknown command");
    }

    /** @see ModuleBase */
    static int print_usage(const char *reason = nullptr);

    void start();

private:
    /** Do a compute and schedule the next cycle. */
    void Run() override;

    /** Do a calculation of the CPU load and publish it. */
    void cpuload();

    /* Stack check only available on Nuttx */
#if defined(__PX4_NUTTX)
    /* Calculate stack usage */
    void stack_usage();

    int _stack_task_index{0};

    uORB::Publication<task_stack_info_s> _task_stack_info_pub{ORB_ID(task_stack_info)};
#endif
    uORB::Publication<cpuload_s> _cpuload_pub{ORB_ID(cpuload)};

#if defined(__PX4_LINUX)
    FILE *_proc_fd = nullptr;
    /* calculate usage directly from clock ticks on Linux */
    clock_t _last_total_time_stamp{};
    clock_t _last_spent_time_stamp{};
#elif defined(__PX4_NUTTX)
    hrt_abstime _last_idle_time{0};
    hrt_abstime _last_idle_time_sample{0};
#endif

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    DEFINE_PARAMETERS(
        (ParamBool<px4::params::SYS_STCK_EN>)_param_sys_stck_en)
};

} // namespace load_mon
