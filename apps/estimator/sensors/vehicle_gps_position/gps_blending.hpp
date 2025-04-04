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
 * @file gps_blending.hpp
 */

#pragma once

#include <hrtimer.h>
#include <matrix/matrix/math.hpp>
#include <defines.h>
#include <uORB/topics/sensor_gps.h>

#include <float.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>

using matrix::Vector2f;
using math::constrain;

using namespace time_literals;

class GpsBlending {
public:
    // Set the GPS timeout to 2s, after which a receiver will be ignored
    static constexpr hrt_abstime GPS_TIMEOUT_US = 2_s;
    static constexpr float       GPS_TIMEOUT_S  = (GPS_TIMEOUT_US / 1e6f);

    GpsBlending()  = default;
    ~GpsBlending() = default;

    // define max number of GPS receivers supported for blending
    static constexpr int GPS_MAX_RECEIVERS_BLEND = 2;

    void setGpsData(const sensor_gps_s &gps_data, int instance) {
        if (instance < GPS_MAX_RECEIVERS_BLEND) {
            _gps_state[instance]   = gps_data;
            _gps_updated[instance] = true;
        }
    }
    void setBlendingUseSpeedAccuracy(bool enabled) {
        _blend_use_spd_acc = enabled;
    }
    void setBlendingUseHPosAccuracy(bool enabled) {
        _blend_use_hpos_acc = enabled;
    }
    void setBlendingUseVPosAccuracy(bool enabled) {
        _blend_use_vpos_acc = enabled;
    }
    void setBlendingTimeConstant(float tau) {
        _blending_time_constant = tau;
    }
    void setPrimaryInstance(int primary) {
        _primary_instance = primary;
    }

    void update(uint64_t hrt_now_us);

    bool isNewOutputDataAvailable() const {
        return _is_new_output_data_available;
    }
    int getNumberOfGpsSuitableForBlending() const {
        return _np_gps_suitable_for_blending;
    }
    const sensor_gps_s &getOutputGpsData() const {
        if (_selected_gps < GPS_MAX_RECEIVERS_BLEND) {
            return _gps_state[_selected_gps];

        } else {
            return _gps_blended_state;
        }
    }
    int getSelectedGps() const {
        return _selected_gps;
    }

private:
    /*
     * Update the internal state estimate for a blended GPS solution that is a weighted average of the phsyical
     * receiver solutions. This internal state cannot be used directly by estimators because if physical receivers
     * have significant position differences, variation in receiver estimated accuracy will cause undesirable
     * variation in the position solution.
     */
    bool blend_gps_data(uint64_t hrt_now_us);

    /*
     * Calculate internal states used to blend GPS data from multiple receivers using weightings calculated
     * by calc_blend_weights()
     */
    sensor_gps_s gps_blend_states(float blend_weights[GPS_MAX_RECEIVERS_BLEND]) const;

    /*
     * The location in gps_blended_state will move around as the relative accuracy changes.
     * To mitigate this effect a low-pass filtered offset from each GPS location to the blended location is
     * calculated.
     */
    void update_gps_offsets(const sensor_gps_s &gps_blended_state);

    /*
     Calculate GPS output that is a blend of the offset corrected physical receiver data
    */
    void calc_gps_blend_output(sensor_gps_s &gps_blended_state, float blend_weights[GPS_MAX_RECEIVERS_BLEND]) const;

    sensor_gps_s _gps_state[GPS_MAX_RECEIVERS_BLEND]{}; ///< internal state data for the physical GPS
    sensor_gps_s _gps_blended_state{};
    bool         _gps_updated[GPS_MAX_RECEIVERS_BLEND]{};
    int          _selected_gps{0};
    int          _np_gps_suitable_for_blending{0};
    int          _primary_instance{0}; ///< if -1, there is no primary isntance and the best receiver is used // TODO: use device_id
    bool         _fallback_allowed{false};

    bool _is_new_output_data_available{false};

    matrix::Vector2f _NE_pos_offset_m[GPS_MAX_RECEIVERS_BLEND]{}; ///< Filtered North,East position offset from GPS instance to blended solution in _output_state.location (m)
    float            _hgt_offset_mm[GPS_MAX_RECEIVERS_BLEND]{};   ///< Filtered height offset from GPS instance relative to blended solution in _output_state.location (mm)

    uint64_t _time_prev_us[GPS_MAX_RECEIVERS_BLEND]{}; ///< the previous value of time_us for that GPS instance - used to detect new data.
    uint8_t  _gps_time_ref_index{0};                   ///< index of the receiver that is used as the timing reference for the blending update
    uint8_t  _gps_newest_index{0};                     ///< index of the physical receiver with the newest data
    uint8_t  _gps_slowest_index{0};                    ///< index of the physical receiver with the slowest update rate
    float    _gps_dt[GPS_MAX_RECEIVERS_BLEND]{};       ///< average time step in seconds.

    bool _blend_use_spd_acc{false};
    bool _blend_use_hpos_acc{false};
    bool _blend_use_vpos_acc{false};

    float _blending_time_constant{0.f};
};
