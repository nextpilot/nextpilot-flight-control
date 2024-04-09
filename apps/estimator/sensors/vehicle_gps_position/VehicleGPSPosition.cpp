/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "VehicleGPSPosition.hpp"

#include <px4_platform_common/log.h>
#include <lib/geo/geo.h>
#include <lib/mathlib/mathlib.h>

namespace sensors {
VehicleGPSPosition::VehicleGPSPosition() :
    ModuleParams(nullptr),
    ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::nav_and_controllers) {
    _vehicle_gps_position_pub.advertise();
}

VehicleGPSPosition::~VehicleGPSPosition() {
    Stop();
    perf_free(_cycle_perf);
}

bool VehicleGPSPosition::Start() {
    // force initial updates
    ParametersUpdate(true);

    ScheduleNow();

    return true;
}

void VehicleGPSPosition::Stop() {
    Deinit();

    // clear all registered callbacks
    for (auto &sub : _sensor_gps_sub) {
        sub.unregisterCallback();
    }
}

void VehicleGPSPosition::ParametersUpdate(bool force) {
    // Check if parameters have changed
    if (_parameter_update_sub.updated() || force) {
        // clear update
        parameter_update_s param_update;
        _parameter_update_sub.copy(&param_update);

        updateParams();

        if (_param_sens_gps_mask.get() == 0) {
            _sensor_gps_sub[0].registerCallback();

        } else {
            for (auto &sub : _sensor_gps_sub) {
                sub.registerCallback();
            }
        }

        _gps_blending.setBlendingUseSpeedAccuracy(_param_sens_gps_mask.get() & BLEND_MASK_USE_SPD_ACC);
        _gps_blending.setBlendingUseHPosAccuracy(_param_sens_gps_mask.get() & BLEND_MASK_USE_HPOS_ACC);
        _gps_blending.setBlendingUseVPosAccuracy(_param_sens_gps_mask.get() & BLEND_MASK_USE_VPOS_ACC);
        _gps_blending.setBlendingTimeConstant(_param_sens_gps_tau.get());
        _gps_blending.setPrimaryInstance(_param_sens_gps_prime.get());
    }
}

void VehicleGPSPosition::Run() {
    perf_begin(_cycle_perf);
    ParametersUpdate();

    // Check all GPS instance
    bool any_gps_updated = false;
    bool gps_updated     = false;

    for (uint8_t i = 0; i < GPS_MAX_RECEIVERS; i++) {
        gps_updated = _sensor_gps_sub[i].updated();

        sensor_gps_s gps_data;

        if (gps_updated) {
            any_gps_updated = true;

            _sensor_gps_sub[i].copy(&gps_data);
            _gps_blending.setGpsData(gps_data, i);

            if (!_sensor_gps_sub[i].registered()) {
                _sensor_gps_sub[i].registerCallback();
            }
        }
    }

    if (any_gps_updated) {
        _gps_blending.update(hrt_absolute_time());

        if (_gps_blending.isNewOutputDataAvailable()) {
            sensor_gps_s gps_output{_gps_blending.getOutputGpsData()};

            // clear device_id if blending
            if (_gps_blending.getSelectedGps() == GpsBlending::GPS_MAX_RECEIVERS_BLEND) {
                gps_output.device_id = 0;
            }

            _vehicle_gps_position_pub.publish(gps_output);
        }
    }

    ScheduleDelayed(300_ms); // backup schedule

    perf_end(_cycle_perf);
}

void VehicleGPSPosition::PrintStatus() {
    PX4_INFO_RAW("[vehicle_gps_position] selected GPS: %d\n", _gps_blending.getSelectedGps());
}

}; // namespace sensors
