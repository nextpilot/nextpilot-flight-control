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

#include "../../HealthAndArmingChecks/HealthAndArmingChecks.hpp"
#include <drivers/drv_hrt.h>
#include <px4_platform_common/events.h>
#include <uORB/topics/actuator_armed.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_status.h>

typedef enum {
    TRANSITION_DENIED      = -1,
    TRANSITION_NOT_CHANGED = 0,
    TRANSITION_CHANGED
} transition_result_t;

using arm_disarm_reason_t = events::px4::enums::arm_disarm_reason_t;

class ArmStateMachine {
public:
    ArmStateMachine()  = default;
    ~ArmStateMachine() = default;

    void forceArmState(uint8_t new_arm_state) {
        _arm_state = new_arm_state;
    }

    transition_result_t
    arming_state_transition(vehicle_status_s &status, const arming_state_t new_arming_state,
                            actuator_armed_s &armed, HealthAndArmingChecks &checks, const bool fRunPreArmChecks,
                            orb_advert_t *mavlink_log_pub, arm_disarm_reason_t calling_reason);

    // Getters
    uint8_t getArmState() const {
        return _arm_state;
    }

    bool isInit() const {
        return (_arm_state == vehicle_status_s::ARMING_STATE_INIT);
    }
    bool isStandby() const {
        return (_arm_state == vehicle_status_s::ARMING_STATE_STANDBY);
    }
    bool isArmed() const {
        return (_arm_state == vehicle_status_s::ARMING_STATE_ARMED);
    }
    bool isShutdown() const {
        return (_arm_state == vehicle_status_s::ARMING_STATE_SHUTDOWN);
    }

    static const char *getArmStateName(uint8_t arming_state);
    const char        *getArmStateName() const {
        return getArmStateName(_arm_state);
    }

private:
    static inline events::px4::enums::arming_state_t getArmStateEvent(uint8_t arming_state);

    uint8_t _arm_state{vehicle_status_s::ARMING_STATE_INIT};

    // This array defines the arming state transitions. The rows are the new state, and the columns
    // are the current state. Using new state and current state you can index into the array which
    // will be true for a valid transition or false for a invalid transition. In some cases even
    // though the transition is marked as true additional checks must be made. See arming_state_transition
    // code for those checks.
    static constexpr bool arming_transitions[vehicle_status_s::ARMING_STATE_MAX][vehicle_status_s::ARMING_STATE_MAX] = {
        //                                                    INIT,  STANDBY, ARMED, STANDBY_ERROR, SHUTDOWN, IN_AIR_RESTORE
        {/* vehicle_status_s::ARMING_STATE_INIT */ true, true, false, true, false, false},
        {/* vehicle_status_s::ARMING_STATE_STANDBY */ true, true, true, false, false, false},
        {/* vehicle_status_s::ARMING_STATE_ARMED */ false, true, true, false, false, true},
        {/* vehicle_status_s::ARMING_STATE_STANDBY_ERROR */ true, true, true, true, false, false},
        {/* vehicle_status_s::ARMING_STATE_SHUTDOWN */ true, true, false, true, true, true},
        {/* vehicle_status_s::ARMING_STATE_IN_AIR_RESTORE */ false, false, false, false, false, false}, // NYI
    };
};
