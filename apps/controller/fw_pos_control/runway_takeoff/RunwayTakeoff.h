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
 * @file RunwayTakeoff.h
 * Runway takeoff handling for fixed-wing UAVs with steerable wheels.
 *
 * @author Roman Bapst <roman@px4.io>
 * @author Andreas Antener <andreas@uaventure.com>
 */

#ifndef RUNWAYTAKEOFF_H
#define RUNWAYTAKEOFF_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <drivers/drv_hrt.h>
#include <px4_platform_common/module_params.h>
#include <mathlib/mathlib.h>
#include <matrix/math.hpp>

namespace runwaytakeoff {

enum RunwayTakeoffState {
    THROTTLE_RAMP = 0, // ramping up throttle
    CLAMPED_TO_RUNWAY, // clamped to runway, controlling yaw directly (wheel or rudder)
    CLIMBOUT,          // climbout to safe height before navigation
    FLY                // navigate freely
};

class __EXPORT RunwayTakeoff : public ModuleParams {
public:
    RunwayTakeoff(ModuleParams *parent) :
        ModuleParams(parent) {
    }
    ~RunwayTakeoff() = default;

    /**
     * @brief Initializes the state machine.
     *
     * @param time_now Absolute time since system boot [us]
     * @param initial_yaw Vehicle yaw angle at time of initialization [us]
     * @param start_pos_global Vehicle global (lat, lon) position at time of initialization [deg]
     */
    void init(const hrt_abstime &time_now, const float initial_yaw, const matrix::Vector2d &start_pos_global);

    /**
     * @brief Updates the state machine based on the current vehicle condition.
     *
     * @param time_now Absolute time since system boot [us]
     * @param takeoff_airspeed Calibrated airspeed setpoint for the takeoff climbout [m/s]
     * @param calibrated_airspeed Vehicle calibrated airspeed [m/s]
     * @param vehicle_altitude Vehicle altitude (AGL) [m]
     * @param clearance_altitude Altitude (AGL) above which we have cleared all occlusions in the runway path [m]
     */
    void update(const hrt_abstime &time_now, const float takeoff_airspeed, const float calibrated_airspeed,
                const float vehicle_altitude, const float clearance_altitude);

    /**
     * @return Current takeoff state
     */
    RunwayTakeoffState getState() {
        return takeoff_state_;
    }

    /**
     * @return The state machine is initialized
     */
    bool isInitialized() {
        return initialized_;
    }

    /**
     * @return Runway takeoff is enabled
     */
    bool runwayTakeoffEnabled() {
        return param_rwto_tkoff_.get();
    }

    /**
     * @return Initial vehicle yaw angle [rad]
     */
    float getInitYaw() {
        return initial_yaw_;
    }

    /**
     * @return The vehicle should control yaw via rudder or nose gear
     */
    bool controlYaw();

    /**
     * @param external_pitch_setpoint Externally commanded pitch angle setpoint (usually from TECS) [rad]
     * @return Pitch angle setpoint (limited while plane is on runway) [rad]
     */
    float getPitch(float external_pitch_setpoint);

    /**
     * @param external_roll_setpoint Externally commanded roll angle setpoint (usually from path navigation) [rad]
     * @return Roll angle setpoint [rad]
     */
    float getRoll(float external_roll_setpoint);

    /**
     * @brief Returns the appropriate yaw angle setpoint.
     *
     * In heading hold mode (_heading_mode == 0), it returns initial yaw as long as it's on the runway.
     * When it has enough ground clearance we start navigation towards WP.
     *
     * @param external_yaw_setpoint Externally commanded yaw angle setpoint [rad]
     * @return Yaw angle setpoint [rad]
     */
    float getYaw(float external_yaw_setpoint);

    /**
     * @brief Returns the throttle setpoint.
     *
     * Ramps up over RWTO_RAMP_TIME to RWTO_MAX_THR until the aircraft lifts off the runway and then
     * ramps from RWTO_MAX_THR to the externally defined throttle setting over the takeoff rotation time
     *
     * @param idle_throttle normalized [0,1]
     * @param external_throttle_setpoint Externally commanded throttle setpoint (usually from TECS), normalized [0,1]
     * @return Throttle setpoint, normalized [0,1]
     */
    float getThrottle(const float idle_throttle, const float external_throttle_setpoint) const;

    /**
     * @param min_pitch_in_climbout Minimum pitch angle during climbout [rad]
     * @param min_pitch Externally commanded minimum pitch angle [rad]
     * @return Minimum pitch angle [rad]
     */
    float getMinPitch(const float min_pitch_in_climbout, const float min_pitch) const;

    /**
     * @param max_pitch Externally commanded maximum pitch angle [rad]
     * @return Maximum pitch angle [rad]
     */
    float getMaxPitch(const float max_pitch) const;

    /**
     * @return Runway takeoff starting position in global frame (lat, lon) [deg]
     */
    const matrix::Vector2d &getStartPosition() const {
        return start_pos_global_;
    };

    // NOTE: this is only to be used for mistaken mode transitions to takeoff while already in air
    void forceSetFlyState() {
        takeoff_state_ = RunwayTakeoffState::FLY;
    }

    /**
     * @return If the attitude / rate control integrators should be continually reset.
     * This is the case during ground roll.
     */
    bool resetIntegrators();

    /**
     * @brief Reset the state machine.
     */
    void reset();

    /**
     * @brief Linearly interpolates between a start and end value over an absolute time span.
     *
     * @param start_value
     * @param end_value
     * @param start_time Absolute start time [us]
     * @param interpolation_time The time span to interpolate over [s]
     * @return interpolated value
     */
    float interpolateValuesOverAbsoluteTime(const float start_value, const float end_value, const hrt_abstime &start_time,
                                            const float interpolation_time) const;

private:
    /**
     * Current state of runway takeoff procedure
     */
    RunwayTakeoffState takeoff_state_{THROTTLE_RAMP};

    /**
     * True if the runway state machine is initialized
     */
    bool initialized_{false};

    /**
     * The absolute time since system boot at which the state machine was intialized [us]
     */
    hrt_abstime time_initialized_{0};

    /**
     * The absolute time the takeoff state transitions from CLAMPED_TO_RUNWAY -> CLIMBOUT [us]
     */
    hrt_abstime takeoff_time_{0};

    /**
     * Initial yaw of the vehicle on first pass through the runway takeoff state machine.
     * used for heading hold mode. [rad]
     */
    float initial_yaw_{0.f};

    /**
     * The global (lat, lon) position of the vehicle on first pass through the runway takeoff state machine. The
     * takeoff path emanates from this point to correct for any GNSS uncertainty from the planned takeoff point. The
     * vehicle should accordingly be set on the center of the runway before engaging the mission. [deg]
     */
    matrix::Vector2d start_pos_global_{};

    DEFINE_PARAMETERS(
        (ParamBool<px4::params::RWTO_TKOFF>)param_rwto_tkoff_,
        (ParamInt<px4::params::RWTO_HDG>)param_rwto_hdg_,
        (ParamFloat<px4::params::RWTO_MAX_THR>)param_rwto_max_thr_,
        (ParamFloat<px4::params::RWTO_PSP>)param_rwto_psp_,
        (ParamFloat<px4::params::RWTO_RAMP_TIME>)param_rwto_ramp_time_,
        (ParamFloat<px4::params::RWTO_ROT_AIRSPD>)param_rwto_rot_airspd_,
        (ParamFloat<px4::params::RWTO_ROT_TIME>)param_rwto_rot_time_)
};

} // namespace runwaytakeoff

#endif // RUNWAYTAKEOFF_H
