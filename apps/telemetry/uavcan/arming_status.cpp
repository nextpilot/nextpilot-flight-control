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
 * @file arming_status.cpp
 *
 * @author Alex Klimaj <alex@arkelectron.com>
 */

#include "arming_status.hpp"

UavcanArmingStatus::UavcanArmingStatus(uavcan::INode &node) :
    _arming_status_pub(node),
    _timer(node) {
    _arming_status_pub.setPriority(uavcan::TransferPriority::Default);
}

int UavcanArmingStatus::init() {
    /*
     * Setup timer and call back function for periodic updates
     */
    if (!_timer.isRunning()) {
        _timer.setCallback(TimerCbBinder(this, &UavcanArmingStatus::periodic_update));
        _timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(1000 / MAX_RATE_HZ));
    }

    return 0;
}

void UavcanArmingStatus::periodic_update(const uavcan::TimerEvent &) {
    actuator_armed_s actuator_armed;

    if (_actuator_armed_sub.update(&actuator_armed)) {
        uavcan::equipment::safety::ArmingStatus cmd;

        if (actuator_armed.lockdown || actuator_armed.manual_lockdown) {
            cmd.status = cmd.STATUS_DISARMED;

        } else if (actuator_armed.armed || actuator_armed.prearmed) {
            cmd.status = cmd.STATUS_FULLY_ARMED;

        } else {
            cmd.status = cmd.STATUS_DISARMED;
        }

        (void)_arming_status_pub.broadcast(cmd);
    }
}
