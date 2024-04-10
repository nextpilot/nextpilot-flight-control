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

#include <px4_platform_common/atomic.h>
#include <px4_platform_common/posix.h>
#include <mavlink_log.h>
#include <uORB/uORB.h>

/**
 * @class WorkerThread
 * low priority background thread, started on demand, used for:
 * - calibration
 * - param saving
 */
class WorkerThread {
public:
    enum class Request {
        GyroCalibration,
        MagCalibration,
        RCTrimCalibration,
        AccelCalibration,
        LevelCalibration,
        AccelCalibrationQuick,
        AirspeedCalibration,
        ESCCalibration,
        MagCalibrationQuick,
        BaroCalibration,

        ParamLoadDefault,
        ParamSaveDefault,
        ParamResetAll,
        ParamResetSensorFactory,
        ParamResetAllConfig
    };

    WorkerThread() = default;
    ~WorkerThread();

    void setMagQuickData(float heading_rad, float lat, float lon);

    void startTask(Request request);

    bool isBusy() const {
        return _state.load() != (int)State::Idle;
    }
    bool hasResult() const {
        return _state.load() == (int)State::Finished;
    }
    int getResultAndReset() {
        _state.store((int)State::Idle);
        return _ret_value;
    }

private:
    enum class State {
        Idle,
        Running,
        Finished
    };

    static void *threadEntryTrampoline(void *arg);
    void         threadEntry();

    px4::atomic_int _state{(int)State::Idle};
    pthread_t       _thread_handle{};
    int             _ret_value{};
    Request         _request;
    orb_advert_t    _mavlink_log_pub{nullptr};

    // extra arguments
    float _heading_radians;
    float _latitude;
    float _longitude;
};
