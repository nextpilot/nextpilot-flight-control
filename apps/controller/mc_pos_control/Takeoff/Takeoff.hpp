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
 * @file Takeoff.hpp
 *
 * A class handling all takeoff states and a smooth ramp up of the motors.
 */

#pragma once

#include <hysteresis/hysteresis.h>
#include <hrtimer.h>
#include <uORB/topics/takeoff_status.h>

using namespace time_literals;
using namespace nextpilot;

enum class TakeoffState {
    disarmed          = takeoff_status_s::TAKEOFF_STATE_DISARMED,
    spoolup           = takeoff_status_s::TAKEOFF_STATE_SPOOLUP,
    ready_for_takeoff = takeoff_status_s::TAKEOFF_STATE_READY_FOR_TAKEOFF,
    rampup            = takeoff_status_s::TAKEOFF_STATE_RAMPUP,
    flight            = takeoff_status_s::TAKEOFF_STATE_FLIGHT
};

class TakeoffHandling {
public:
    TakeoffHandling()  = default;
    ~TakeoffHandling() = default;

    // initialize parameters
    void setSpoolupTime(const float seconds) {
        _spoolup_time_hysteresis.set_hysteresis_time_from(false, seconds * 1_s);
    }
    void setTakeoffRampTime(const float seconds) {
        _takeoff_ramp_time = seconds;
    }

    /**
     * Calculate a vertical velocity to initialize the takeoff ramp
     * that when passed to the velocity controller results in a zero throttle setpoint.
     * @param hover_thrust normalized thrsut value with which the vehicle hovers
     * @param velocity_p_gain proportional gain of the velocity controller to calculate the thrust
     */
    void generateInitialRampValue(const float velocity_p_gain);

    /**
     * Update the state for the takeoff.
     * Has to be called also when not flying altitude controlled to skip the takeoff and not do it in flight when switching mode.
     */
    void updateTakeoffState(const bool armed, const bool landed, const bool want_takeoff,
                            const float takeoff_desired_vz, const bool skip_takeoff, const hrt_abstime &now_us);

    /**
     * Update and return the velocity constraint ramp value during takeoff.
     * By ramping up _takeoff_ramp_vz during the takeoff and using it to constain the maximum climb rate a smooth takeoff behavior is achieved.
     * Returns zero on the ground and takeoff_desired_vz in flight.
     * @param dt time in seconds since the last call/loop iteration
     * @param takeoff_desired_vz end value for the velocity ramp
     * @return true if setpoint has updated correctly
     */
    float updateRamp(const float dt, const float takeoff_desired_vz);

    TakeoffState getTakeoffState() {
        return _takeoff_state;
    }

private:
    TakeoffState _takeoff_state = TakeoffState::disarmed;

    Hysteresis _spoolup_time_hysteresis{false}; ///< becomes true COM_SPOOLUP_TIME seconds after the vehicle was armed

    float _takeoff_ramp_time{0.f};
    float _takeoff_ramp_vz_init{0.f};  ///< verticval velocity resulting in zero thrust
    float _takeoff_ramp_progress{0.f}; ///< asecnding from 0 to 1
};
