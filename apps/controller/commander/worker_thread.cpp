/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "worker_thread.hpp"
#include "accelerometer_calibration.h"
#include "airspeed_calibration.h"
#include "calibration_routines.h"
#include "baro_calibration.h"
#include "esc_calibration.h"
#include "gyro_calibration.h"
#include "level_calibration.h"
#include "mag_calibration.h"
#include "rc_calibration.h"

// #include <events/events.h>
#include <rtdbg.h>
#include <px4_platform_common/shutdown.h>
#include <param/param.h>

using namespace time_literals;

WorkerThread::~WorkerThread() {
    if (_state.load() == (int)State::Running) {
        /* wait for thread to complete */
        int ret = pthread_join(_thread_handle, nullptr);

        if (ret) {
            PX4_ERR("join failed: %d", ret);
        }
    }
}

void WorkerThread::startTask(Request request) {
    if (isBusy()) {
        return;
    }

    _request = request;

    /* initialize low priority thread */
    pthread_attr_t low_prio_attr;
    pthread_attr_init(&low_prio_attr);
    pthread_attr_setstacksize(&low_prio_attr, PX4_STACK_ADJUSTED(4804));

    struct sched_param param;
    pthread_attr_getschedparam(&low_prio_attr, &param);

    /* low priority */
    param.sched_priority = SCHED_PRIORITY_DEFAULT - 50;
    pthread_attr_setschedparam(&low_prio_attr, &param);

    int ret = pthread_create(&_thread_handle, &low_prio_attr, &threadEntryTrampoline, this);
    pthread_attr_destroy(&low_prio_attr);

    if (ret == 0) {
        _state.store((int)State::Running);

    } else {
        PX4_ERR("Failed to start thread (%i)", ret);
        _state.store((int)State::Finished);
        _ret_value = ret;
    }
}

void *WorkerThread::threadEntryTrampoline(void *arg) {
    WorkerThread *worker_thread = (WorkerThread *)arg;
    worker_thread->threadEntry();
    return nullptr;
}

void WorkerThread::threadEntry() {
    px4_prctl(PR_SET_NAME, "commander_low_prio", px4_getpid());

    switch (_request) {
    case Request::GyroCalibration:
        _ret_value = do_gyro_calibration(&_mavlink_log_pub);
        break;

    case Request::MagCalibration:
        _ret_value = do_mag_calibration(&_mavlink_log_pub);
        break;

    case Request::RCTrimCalibration:
        _ret_value = do_trim_calibration(&_mavlink_log_pub);
        break;

    case Request::AccelCalibration:
        _ret_value = do_accel_calibration(&_mavlink_log_pub);
        break;

    case Request::LevelCalibration:
        _ret_value = do_level_calibration(&_mavlink_log_pub);
        break;

    case Request::AccelCalibrationQuick:
        _ret_value = do_accel_calibration_quick(&_mavlink_log_pub);
        break;

    case Request::AirspeedCalibration:
        _ret_value = do_airspeed_calibration(&_mavlink_log_pub);
        break;

    case Request::ESCCalibration:
        _ret_value = do_esc_calibration(&_mavlink_log_pub);
        break;

    case Request::MagCalibrationQuick:
        _ret_value = do_mag_calibration_quick(&_mavlink_log_pub, _heading_radians, _latitude, _longitude);
        break;

    case Request::BaroCalibration:
        _ret_value = do_baro_calibration(&_mavlink_log_pub);
        break;

    case Request::ParamLoadDefault:
        _ret_value = param_load_default();

        if (_ret_value != 0) {
            mavlink_log_critical(&_mavlink_log_pub, "Error loading settings\t");
            events::send(events::ID("commander_load_param_failed"), events::Log::Critical, "Error loading settings");
        }

        break;

    case Request::ParamSaveDefault:
        _ret_value = param_save_default();

        if (_ret_value != 0) {
            mavlink_log_critical(&_mavlink_log_pub, "Error saving settings\t");
            events::send(events::ID("commander_save_param_failed"), events::Log::Critical, "Error saving settings");
        }

        break;

    case Request::ParamResetAll:
        param_reset_all();
        _ret_value = 0;
        break;

    case Request::ParamResetSensorFactory: {
        const char *reset_cal[] = {"CAL_ACC*", "CAL_GYRO*", "CAL_MAG*"};
        param_reset_specific(reset_cal, sizeof(reset_cal) / sizeof(reset_cal[0]));
        _ret_value = param_save_default();
#if defined(CONFIG_BOARDCTL_RESET)
        px4_reboot_request(false, 400_ms);
#endif // CONFIG_BOARDCTL_RESET
        break;
    }

    case Request::ParamResetAllConfig: {
        const char *exclude_list[] = {
            "LND_FLIGHT_T_HI",
            "LND_FLIGHT_T_LO",
            "COM_FLIGHT_UUID"};
        param_reset_excludes(exclude_list, sizeof(exclude_list) / sizeof(exclude_list[0]));
        _ret_value = 0;
        break;
    }
    }

    _state.store((int)State::Finished); // set this last to signal the main thread we're done
}

void WorkerThread::setMagQuickData(float heading_rad, float lat, float lon) {
    _heading_radians = heading_rad;
    _latitude        = lat;
    _longitude       = lon;
}
