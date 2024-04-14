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
 * @file beep.hpp
 *
 * @author CUAVcaijie <caijie@cuav.net>
 *
 * @brief Control CAN buzzer by subscribing to tune_control
 *
 */

#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/indication/BeepCommand.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/tune_control.h>
#include <lib/tunes/tunes.h>

class UavcanBeepController {
public:
    UavcanBeepController(uavcan::INode &node);

    /*
     * setup periodic updater
     */
    int init();

private:
    /*
     * Max update rate to avoid excessive bus traffic
     */
    static constexpr unsigned MAX_RATE_HZ = 100;

    /*
     * Setup timer and call back function for periodic updates
     */
    void periodic_update(const uavcan::TimerEvent &);

    typedef uavcan::MethodBinder<UavcanBeepController *, void (UavcanBeepController::*)(const uavcan::TimerEvent &)>
        TimerCbBinder;

    /*
     * Subscription tune_control
     */
    uORB::Subscription _tune_control_sub{ORB_ID(tune_control)};

    /*
     * Publish CAN Beep
     */
    uavcan::Publisher<uavcan::equipment::indication::BeepCommand> _beep_pub;

    uavcan::TimerEventForwarder<TimerCbBinder> _timer;

    hrt_abstime    _interval_timestamp{0};
    tune_control_s _tune{};
    Tunes          _tunes{};
    unsigned int   _silence_length{0}; ///< If nonzero, silence before next note.
    unsigned int   _frequency{0};
    unsigned int   _duration{0};
};
