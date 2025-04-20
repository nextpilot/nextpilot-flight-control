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
 * @file camera_capture.hpp
 *
 */

#pragma once

#include <drivers/drv_hrt.h>
#include <drivers/drv_input_capture.h>
#include <drivers/drv_pwm_output.h>
#include <lib/param/param.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/tasks.h>
#include <px4_platform_common/workqueue.h>
#include <px4_platform_common/px4_work_queue/WorkItemScheduled.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/camera_trigger.h>
#include <uORB/topics/pps_capture.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>

class CameraCapture : public px4::WorkItemScheduled {
public:
    /**
	 * Constructor
	 */
    CameraCapture();

    /**
	 * Destructor, also kills task.
	 */
    ~CameraCapture();

    /**
	 * Start the task.
	 */
    int start();

    /**
	 * Stop the task.
	 */
    void stop();

    void status();

    // Low-rate command handling loop
    void Run() override;

    static void capture_trampoline(void *context, uint32_t chan_index, hrt_abstime edge_time, uint32_t edge_state,
                                   uint32_t overflow);

    void set_capture_control(bool enabled);

    void reset_statistics(bool reset_seq);

    void publish_trigger();


    static struct work_s _work_publisher;

private:
    int _capture_channel = 5; ///< by default, use FMU output 6

    // Publishers
    uORB::Publication<vehicle_command_ack_s> _command_ack_pub{ORB_ID(vehicle_command_ack)};
    uORB::Publication<camera_trigger_s>      _trigger_pub{ORB_ID(camera_trigger)};

    // Subscribers
    uORB::Subscription _command_sub{ORB_ID(vehicle_command)};
    uORB::Subscription _pps_capture_sub{ORB_ID(pps_capture)};

    // Trigger Buffer
    struct _trig_s {
        uint32_t    chan_index;
        hrt_abstime hrt_edge_time;
        uint32_t    edge_state;
        uint32_t    overflow;
    } _trigger{};

    bool _capture_enabled{false};
    bool _gpio_capture{false};

    // Parameters
    param_t _p_strobe_delay{PARAM_INVALID};
    float   _strobe_delay{0.0f};
    param_t _p_camera_capture_mode{PARAM_INVALID};
    int32_t _camera_capture_mode{0};
    param_t _p_camera_capture_edge{PARAM_INVALID};
    int32_t _camera_capture_edge{0};

    // Signal capture statistics
    uint32_t    _capture_seq{0};
    hrt_abstime _last_trig_begin_time{0};
    hrt_abstime _last_exposure_time{0};
    hrt_abstime _last_trig_time{0};
    uint32_t    _capture_overflows{0};

    hrt_abstime       _pps_hrt_timestamp{0};
    uint64_t          _pps_rtc_timestamp{0};
    uint8_t           _trigger_rate_exceeded_counter{0};
    px4::atomic<bool> _trigger_rate_failure{false};

    orb_advert_t _mavlink_log_pub{nullptr};

    // Signal capture callback
    void capture_callback(uint32_t chan_index, hrt_abstime edge_time, uint32_t edge_state, uint32_t overflow);

    // GPIO interrupt routine
    static int gpio_interrupt_routine(int irq, void *context, void *arg);

    // Signal capture publish
    static void publish_trigger_trampoline(void *arg);
};
