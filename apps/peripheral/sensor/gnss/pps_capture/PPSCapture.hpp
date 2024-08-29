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

#include <drivers/drv_hrt.h>
#include <px4_arch/micro_hal.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/pps_capture.h>
#include <uORB/topics/sensor_gps.h>

using namespace time_literals;

class PPSCapture : public ModuleBase<PPSCapture>, public px4::ScheduledWorkItem {
public:
    PPSCapture();
    virtual ~PPSCapture();

    /** @see ModuleBase */
    static int task_spawn(int argc, char *argv[]);

    /** @see ModuleBase */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleBase */
    static int print_usage(const char *reason = nullptr);

    bool init();

    static int gpio_interrupt_callback(int irq, void *context, void *arg);

    /** PPSCapture is an interrupt-driven task and needs to be manually stopped */
    static void stop();

private:
    void Run() override;

    int                              _channel{-1};
    uint32_t                         _pps_capture_gpio{0};
    uORB::Publication<pps_capture_s> _pps_capture_pub{ORB_ID(pps_capture)};
    uORB::Subscription               _sensor_gps_sub{ORB_ID(sensor_gps)};
    orb_advert_t                     _mavlink_log_pub{nullptr};

    hrt_abstime _hrt_timestamp{0};

    hrt_abstime       _last_gps_timestamp{0};
    uint64_t          _last_gps_utc_timestamp{0};
    uint8_t           _pps_rate_exceeded_counter{0};
    px4::atomic<bool> _pps_rate_failure{false};
};
