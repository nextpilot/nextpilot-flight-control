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
 * @file safety_state.hpp
 *
 * @author CUAVcaijie <caijie@cuav.net>
 *
 * @brief According to actuator_armed to control the CAN SafetyState led
 */

#pragma once

#include <uavcan/uavcan.hpp>
#include <ardupilot/indication/SafetyState.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/actuator_armed.h>

class UavcanSafetyState {
public:
    UavcanSafetyState(uavcan::INode &node);

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

    typedef uavcan::MethodBinder<UavcanSafetyState *, void (UavcanSafetyState::*)(const uavcan::TimerEvent &)>
        TimerCbBinder;

    /*
     * Publish CAN Safety state led
     */
    uavcan::Publisher<ardupilot::indication::SafetyState> _safety_state_pub;

    uavcan::TimerEventForwarder<TimerCbBinder> _timer;

    uORB::Subscription _actuator_armed_sub{ORB_ID(actuator_armed)};
};
