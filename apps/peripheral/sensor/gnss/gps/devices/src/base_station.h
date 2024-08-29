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
 * @file base_station.h
 *
 * @author Beat Küng <beat-kueng@gmx.net>
 *
 */

#pragma once

#include "gps_helper.h"

/**
 * @class GPSBaseStationSupport
 * GPS driver base class with Base Station Support
 */
class GPSBaseStationSupport : public GPSHelper {
public:
    GPSBaseStationSupport(GPSCallbackPtr callback, void *callback_user) :
        GPSHelper(callback, callback_user) {
    }

    virtual ~GPSBaseStationSupport() = default;

    /**
	 * set survey-in specs for RTK base station setup (for finding an accurate base station position
	 * by averaging the position measurements over time).
	 * @param survey_in_acc_limit minimum accuracy in 0.1mm
	 * @param survey_in_min_dur minimum duration in seconds
	 */
    void setSurveyInSpecs(uint32_t survey_in_acc_limit, uint32_t survey_in_min_dur) {
        _base_settings.type                         = BaseSettingsType::survey_in;
        _base_settings.settings.survey_in.acc_limit = survey_in_acc_limit;
        _base_settings.settings.survey_in.min_dur   = survey_in_min_dur;
    }

    /**
	 * Set a fixed base station position. This can be used if the base position is already known to
	 * avoid doing a survey-in.
	 * @param latitude [deg]
	 * @param longitude [deg]
	 * @param altitude [m]
	 * @param position_accuracy 3D position accuracy (set to 0 if unknown) [mm]
	 */
    void setBasePosition(double latitude, double longitude, float altitude, float position_accuracy) {
        _base_settings.type                                      = BaseSettingsType::fixed_position;
        _base_settings.settings.fixed_position.latitude          = latitude;
        _base_settings.settings.fixed_position.longitude         = longitude;
        _base_settings.settings.fixed_position.altitude          = altitude;
        _base_settings.settings.fixed_position.position_accuracy = position_accuracy;
    }

protected:
    enum class BaseSettingsType : uint8_t {
        survey_in,
        fixed_position
    };

    struct SurveyInSettings {
        uint32_t acc_limit;
        uint32_t min_dur;
    };

    struct FixedPositionSettings {
        double latitude;
        double longitude;
        float  altitude;
        float  position_accuracy;
    };

    struct BaseSettings {
        BaseSettingsType type;

        union {
            SurveyInSettings      survey_in;
            FixedPositionSettings fixed_position;
        } settings;
    };

    BaseSettings _base_settings;
};
