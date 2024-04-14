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
 * @file safety_state.cpp
 *
 * @author CUAVcaijie <caijie@cuav.net>
 */

#include "safety_state.hpp"

UavcanSafetyState::UavcanSafetyState(uavcan::INode &node) :
    _safety_state_pub(node),
    _timer(node) {
    _safety_state_pub.setPriority(uavcan::TransferPriority::Default);
}

int UavcanSafetyState::init() {
    /*
     * Setup timer and call back function for periodic updates
     */
    if (!_timer.isRunning()) {
        _timer.setCallback(TimerCbBinder(this, &UavcanSafetyState::periodic_update));
        _timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(1000 / MAX_RATE_HZ));
    }

    return 0;
}

void UavcanSafetyState::periodic_update(const uavcan::TimerEvent &) {
    actuator_armed_s actuator_armed;

    if (_actuator_armed_sub.update(&actuator_armed)) {
        ardupilot::indication::SafetyState cmd;

        if (actuator_armed.armed || actuator_armed.prearmed) {
            cmd.status = cmd.STATUS_SAFETY_OFF;

        } else {
            cmd.status = cmd.STATUS_SAFETY_ON;
        }

        (void)_safety_state_pub.broadcast(cmd);
    }
}
