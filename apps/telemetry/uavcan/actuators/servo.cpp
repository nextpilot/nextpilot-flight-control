/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "servo.hpp"
#include <systemlib/err.h>
#include <drivers/drv_hrt.h>

using namespace time_literals;

UavcanServoController::UavcanServoController(uavcan::INode &node) :
    _node(node),
    _uavcan_pub_array_cmd(node) {
    _uavcan_pub_array_cmd.setPriority(UAVCAN_COMMAND_TRANSFER_PRIORITY);
}

void UavcanServoController::update_outputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS], unsigned num_outputs) {
    uavcan::equipment::actuator::ArrayCommand msg;

    for (unsigned i = 0; i < num_outputs; ++i) {
        uavcan::equipment::actuator::Command cmd;
        cmd.actuator_id   = i;
        cmd.command_type  = uavcan::equipment::actuator::Command::COMMAND_TYPE_UNITLESS;
        cmd.command_value = (float)outputs[i] / 500.f - 1.f; // [-1, 1]

        msg.commands.push_back(cmd);
    }

    _uavcan_pub_array_cmd.broadcast(msg);
}
