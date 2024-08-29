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
 * @file PPSCapture.cpp
 *
 * This is driver for capturing GNSS Pulse Per Second (PPS) signal.
 *
 */

#include "PPSCapture.hpp"
#include <px4_arch/io_timer.h>
#include <board_config.h>
#include <parameters/param.h>
#include <px4_platform_common/events.h>
#include <systemlib/mavlink_log.h>

PPSCapture::PPSCapture() :
    ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::hp_default) {
    _pps_capture_pub.advertise();
}

PPSCapture::~PPSCapture() {
    if (_channel >= 0) {
        io_timer_unallocate_channel(_channel);
        px4_arch_gpiosetevent(_pps_capture_gpio, false, false, false, nullptr, nullptr);
    }
}

bool PPSCapture::init() {
    bool success = false;

    for (unsigned i = 0; i < 16; ++i) {
        char param_name[17];
        snprintf(param_name, sizeof(param_name), "%s_%s%d", PARAM_PREFIX, "FUNC", i + 1);
        param_t function_handle = param_find(param_name);
        int32_t function;

        if (function_handle != PARAM_INVALID && param_get(function_handle, &function) == 0) {
            if (function == 2064) { // PPS_Input
                _channel = i;
            }
        }
    }

#if defined(PPS_CAPTURE_CHANNEL)

    if (_channel == -1) {
        _channel = PPS_CAPTURE_CHANNEL;
    }

#endif

    if (_channel == -1) {
        PX4_WARN("No pps channel configured");
        return false;
    }

    int ret = io_timer_allocate_channel(_channel, IOTimerChanMode_PPS);

    if (ret != PX4_OK) {
        PX4_ERR("gpio alloc failed (%i) for PPS at channel (%d)", ret, _channel);
        return false;
    }

    _pps_capture_gpio = PX4_MAKE_GPIO_EXTI(io_timer_channel_get_as_pwm_input(_channel));
    int ret_val       = px4_arch_gpiosetevent(_pps_capture_gpio, true, false, true, &PPSCapture::gpio_interrupt_callback, this);

    if (ret_val == PX4_OK) {
        success = true;
    }

    return success;
}

void PPSCapture::Run() {
    if (should_exit()) {
        exit_and_cleanup();
        return;
    }

    sensor_gps_s sensor_gps;

    if (_sensor_gps_sub.update(&sensor_gps)) {
        _last_gps_utc_timestamp = sensor_gps.time_utc_usec;
        _last_gps_timestamp     = sensor_gps.timestamp;
    }

    pps_capture_s pps_capture;
    pps_capture.timestamp                 = _hrt_timestamp;
    pps_capture.pps_rate_exceeded_counter = _pps_rate_exceeded_counter;
    // GPS UTC time when the GPIO interrupt was triggered
    // Last UTC time received from the GPS + elapsed time to the PPS interrupt
    uint64_t gps_utc_time = _last_gps_utc_timestamp + (_hrt_timestamp - _last_gps_timestamp);

    // (For ubx F9P) The rising edge of the PPS pulse is aligned to the top of second GPS time base.
    // So, remove the fraction of second and shift to the next second. The interrupt is triggered
    // before the matching timestamp is received via a UART message, which means the last received GPS time is always
    // behind.
    pps_capture.rtc_timestamp = gps_utc_time - (gps_utc_time % USEC_PER_SEC) + USEC_PER_SEC;

    _pps_capture_pub.publish(pps_capture);

    if (_pps_rate_failure.load()) {
        mavlink_log_warning(&_mavlink_log_pub, "Hardware fault: GPS PPS disabled\t");
        events::send(events::ID("pps_capture_pps_rate_exceeded"),
                     events::Log::Error, "Hardware fault: GPS PPS disabled");
        _pps_rate_failure.store(false);
    }
}

int PPSCapture::gpio_interrupt_callback(int irq, void *context, void *arg) {
    PPSCapture *instance = static_cast<PPSCapture *>(arg);

    hrt_abstime interrupt_time = hrt_absolute_time();

    if ((interrupt_time - instance->_hrt_timestamp) < 50_ms) {
        ++(instance->_pps_rate_exceeded_counter);

        if (instance->_pps_rate_exceeded_counter >= 10) {
            // Trigger rate too high, stop future interrupts
            px4_arch_gpiosetevent(instance->_pps_capture_gpio, false, false, false, nullptr, nullptr);
            instance->_pps_rate_failure.store(true);
        }
    }

    instance->_hrt_timestamp = interrupt_time;
    instance->ScheduleNow(); // schedule work queue to publish PPS captured time

    return PX4_OK;
}

int PPSCapture::task_spawn(int argc, char *argv[]) {
    PPSCapture *instance = new PPSCapture();

    if (instance) {
        _object.store(instance);
        _task_id = task_id_is_work_queue;

        if (instance->init()) {
            return PX4_OK;
        }

    } else {
        PX4_ERR("alloc failed");
    }

    delete instance;
    _object.store(nullptr);
    _task_id = -1;

    return PX4_ERROR;
}

int PPSCapture::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int PPSCapture::print_usage(const char *reason) {
    if (reason) {
        PX4_WARN("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description
This implements capturing PPS information from the GNSS module and calculates the drift between PPS and Real-time clock.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("pps_capture", "driver");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

void PPSCapture::stop() {
    exit_and_cleanup();
}

extern "C" __EXPORT int pps_capture_main(int argc, char *argv[]) {
    if (argc >= 2 && !strcmp(argv[1], "stop") && PPSCapture::is_running()) {
        PPSCapture::stop();
    }

    return PPSCapture::main(argc, argv);
}
