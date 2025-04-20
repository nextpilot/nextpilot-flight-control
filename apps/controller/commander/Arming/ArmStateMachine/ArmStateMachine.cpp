/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ArmStateMachine.hpp"

#include <systemlib/mavlink_log.h>

constexpr bool
    ArmStateMachine::arming_transitions[vehicle_status_s::ARMING_STATE_MAX][vehicle_status_s::ARMING_STATE_MAX];

transition_result_t ArmStateMachine::arming_state_transition(vehicle_status_s    &status,
                                                             const arming_state_t new_arming_state, actuator_armed_s &armed, HealthAndArmingChecks &checks,
                                                             const bool fRunPreArmChecks, orb_advert_t *mavlink_log_pub, arm_disarm_reason_t calling_reason) {
    // Double check that our static arrays are still valid
    static_assert(vehicle_status_s::ARMING_STATE_INIT == 0, "ARMING_STATE_INIT == 0");
    static_assert(vehicle_status_s::ARMING_STATE_IN_AIR_RESTORE == vehicle_status_s::ARMING_STATE_MAX - 1,
                  "ARMING_STATE_IN_AIR_RESTORE == ARMING_STATE_MAX - 1");

    transition_result_t ret               = TRANSITION_DENIED;
    bool                feedback_provided = false;

    /* only check transition if the new state is actually different from the current one */
    if (new_arming_state == _arm_state) {
        ret = TRANSITION_NOT_CHANGED;

    } else {
        // Check that we have a valid state transition
        bool valid_transition = arming_transitions[new_arming_state][_arm_state];

        // Preflight check
        if (valid_transition && (new_arming_state == vehicle_status_s::ARMING_STATE_ARMED) && fRunPreArmChecks && !(status.hil_state == vehicle_status_s::HIL_STATE_ON) && (_arm_state != vehicle_status_s::ARMING_STATE_IN_AIR_RESTORE)) {
            checks.update();

            if (!checks.canArm(status.nav_state)) {
                feedback_provided = true; // Preflight checks report error messages
                valid_transition  = false;
            }
        }

        if (status.hil_state == vehicle_status_s::HIL_STATE_ON) {
            /* enforce lockdown in HIL */
            armed.lockdown = true;

            /* recover from a prearm fail */
            if (_arm_state == vehicle_status_s::ARMING_STATE_STANDBY_ERROR) {
                _arm_state = vehicle_status_s::ARMING_STATE_STANDBY;
            }

            // HIL can always go to standby
            if (new_arming_state == vehicle_status_s::ARMING_STATE_STANDBY) {
                valid_transition = true;
            }
        }

        // Finish up the state transition
        if (valid_transition) {
            ret = TRANSITION_CHANGED;

            // Record arm/disarm reason
            if (isArmed() && (new_arming_state != vehicle_status_s::ARMING_STATE_ARMED)) { // disarm transition
                status.latest_disarming_reason = (uint8_t)calling_reason;

            } else if (!isArmed() && (new_arming_state == vehicle_status_s::ARMING_STATE_ARMED)) { // arm transition
                status.latest_arming_reason = (uint8_t)calling_reason;
            }

            // Switch state
            _arm_state = new_arming_state;

            if (isArmed()) {
                status.armed_time = hrt_absolute_time();

            } else {
                status.armed_time = 0;
            }
        }
    }

    if (ret == TRANSITION_DENIED) {
        /* print to MAVLink and console if we didn't provide any feedback yet */
        if (!feedback_provided) {
            // FIXME: this catch-all does not provide helpful information to the user
            mavlink_log_critical(mavlink_log_pub, "Transition denied: %s to %s\t",
                                 getArmStateName(_arm_state), getArmStateName(new_arming_state));
            events::send<events::px4::enums::arming_state_t, events::px4::enums::arming_state_t>(
                events::ID("commander_transition_denied"), events::Log::Critical,
                "Arming state transition denied: {1} to {2}",
                getArmStateEvent(_arm_state), getArmStateEvent(new_arming_state));
        }
    }

    return ret;
}

const char *ArmStateMachine::getArmStateName(uint8_t arming_state) {
    switch (arming_state) {
    case vehicle_status_s::ARMING_STATE_INIT:
        return "Init";

    case vehicle_status_s::ARMING_STATE_STANDBY:
        return "Standby";

    case vehicle_status_s::ARMING_STATE_ARMED:
        return "Armed";

    case vehicle_status_s::ARMING_STATE_STANDBY_ERROR:
        return "Standby error";

    case vehicle_status_s::ARMING_STATE_SHUTDOWN:
        return "Shutdown";

    case vehicle_status_s::ARMING_STATE_IN_AIR_RESTORE:
        return "In-air restore";

    default:
        return "Unknown";
    }

    static_assert(vehicle_status_s::ARMING_STATE_MAX - 1 == vehicle_status_s::ARMING_STATE_IN_AIR_RESTORE,
                  "enum def mismatch");
}

events::px4::enums::arming_state_t ArmStateMachine::getArmStateEvent(uint8_t arming_state) {
    switch (arming_state) {
    case vehicle_status_s::ARMING_STATE_INIT:
        return events::px4::enums::arming_state_t::init;

    case vehicle_status_s::ARMING_STATE_STANDBY:
        return events::px4::enums::arming_state_t::standby;

    case vehicle_status_s::ARMING_STATE_ARMED:
        return events::px4::enums::arming_state_t::armed;

    case vehicle_status_s::ARMING_STATE_STANDBY_ERROR:
        return events::px4::enums::arming_state_t::standby_error;

    case vehicle_status_s::ARMING_STATE_SHUTDOWN:
        return events::px4::enums::arming_state_t::shutdown;

    case vehicle_status_s::ARMING_STATE_IN_AIR_RESTORE:
        return events::px4::enums::arming_state_t::inair_restore;
    }

    static_assert(vehicle_status_s::ARMING_STATE_MAX - 1 == (int)events::px4::enums::arming_state_t::inair_restore,
                  "enum def mismatch");

    return events::px4::enums::arming_state_t::init;
}
