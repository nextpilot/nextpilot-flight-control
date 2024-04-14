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
 * @file arming_status.hpp
 *
 * @author Alex Klimaj <alex@arkelectron.com>
 *
 * @brief Publish actuator_armed to CAN ArmingStatus message
 */

#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/safety/ArmingStatus.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/actuator_armed.h>

class UavcanArmingStatus {
public:
    UavcanArmingStatus(uavcan::INode &node);

    /*
     * setup periodic updater
     */
    int init();

private:
    /*
     * Max update rate to avoid exessive bus traffic
     */
    static constexpr unsigned MAX_RATE_HZ = 10;

    /*
     * Setup timer and call back function for periodic updates
     */
    void periodic_update(const uavcan::TimerEvent &);

    typedef uavcan::MethodBinder<UavcanArmingStatus *, void (UavcanArmingStatus::*)(const uavcan::TimerEvent &)>
        TimerCbBinder;

    /*
     * Publish CAN ArmingState
     */
    uavcan::Publisher<uavcan::equipment::safety::ArmingStatus> _arming_status_pub;

    uavcan::TimerEventForwarder<TimerCbBinder> _timer;

    uORB::Subscription _actuator_armed_sub{ORB_ID(actuator_armed)};
};
