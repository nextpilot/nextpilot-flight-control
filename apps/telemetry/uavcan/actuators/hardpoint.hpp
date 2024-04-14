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
 * @file hardpoint.hpp
 *
 * @author Andreas Jochum <Andreas@NicaDrone.com>
 */

#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/hardpoint/Command.hpp>
#include <uavcan/equipment/hardpoint/Status.hpp>
#include <perf/perf_counter.h>

/**
 * @brief The UavcanHardpointController class
 */

class UavcanHardpointController {
public:
    UavcanHardpointController(uavcan::INode &node);
    ~UavcanHardpointController();

    /*
     * setup periodic updater
     */
    int init();

    /*
     * set command
     */
    void set_command(uint8_t hardpoint_id, uint16_t command);

private:
    /*
     * Max update rate to avoid exessive bus traffic
     */
    static constexpr unsigned MAX_RATE_HZ = 1; ///< XXX make this configurable

    uavcan::equipment::hardpoint::Command _cmd;

    void periodic_update(const uavcan::TimerEvent &);

    typedef uavcan::MethodBinder<UavcanHardpointController *, void (UavcanHardpointController::*)(const uavcan::TimerEvent &)>
        TimerCbBinder;

    pthread_mutex_t _node_mutex;
    /*
     * libuavcan related things
     */
    uavcan::INode                                           &_node;
    uavcan::Publisher<uavcan::equipment::hardpoint::Command> _uavcan_pub_raw_cmd;
    uavcan::TimerEventForwarder<TimerCbBinder>               _timer;
};
