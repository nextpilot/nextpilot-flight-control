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
 * @file hardpoint.cpp
 *
 * @author Andreas Jochum <Andreas@NicaDrone.com>
 */

#include "hardpoint.hpp"
#include <systemlib/err.h>

UavcanHardpointController::UavcanHardpointController(uavcan::INode &node) :
    _node(node),
    _uavcan_pub_raw_cmd(node),
    _timer(node) {
    _uavcan_pub_raw_cmd.setPriority(uavcan::TransferPriority::MiddleLower);
}

UavcanHardpointController::~UavcanHardpointController() {
}

int UavcanHardpointController::init() {
    /*
     * Setup timer and call back function for periodic updates
     */
    _timer.setCallback(TimerCbBinder(this, &UavcanHardpointController::periodic_update));
    return 0;
}

void UavcanHardpointController::set_command(uint8_t hardpoint_id, uint16_t command) {
    _cmd.command      = command;
    _cmd.hardpoint_id = hardpoint_id;

    /*
     * Publish the command message to the bus
     */
    _uavcan_pub_raw_cmd.broadcast(_cmd);

    /*
     * Start the periodic update timer after a command is set
     */
    if (!_timer.isRunning()) {
        _timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(1000 / MAX_RATE_HZ));
    }
}

void UavcanHardpointController::periodic_update(const uavcan::TimerEvent &) {
    // Broadcast command at MAX_RATE_HZ
    _uavcan_pub_raw_cmd.broadcast(_cmd);
}
