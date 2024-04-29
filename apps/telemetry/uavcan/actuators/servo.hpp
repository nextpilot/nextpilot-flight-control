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

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/actuator/Status.hpp>
#include <uavcan/equipment/actuator/ArrayCommand.hpp>
#include <perf/perf_counter.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/actuator_outputs.h>
#include <hrtimer.h>
#include <mixer_module/mixer_module.hpp>

class UavcanServoController {
public:
    static constexpr int      MAX_ACTUATORS                    = 8;
    static constexpr unsigned MAX_RATE_HZ                      = 50;
    static constexpr unsigned UAVCAN_COMMAND_TRANSFER_PRIORITY = 6; ///< 0..31, inclusive, 0 - highest, 31 - lowest

    UavcanServoController(uavcan::INode &node);
    ~UavcanServoController() = default;

    void update_outputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS], unsigned num_outputs);

private:
    uavcan::INode                                               &_node;
    uavcan::Publisher<uavcan::equipment::actuator::ArrayCommand> _uavcan_pub_array_cmd;
};
