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
 * @file esc.hpp
 *
 * UAVCAN <--> ORB bridge for ESC messages:
 *     uavcan.equipment.esc.RawCommand
 *     uavcan.equipment.esc.RPMCommand
 *     uavcan.equipment.esc.Status
 *
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/esc/RawCommand.hpp>
#include <uavcan/equipment/esc/Status.hpp>
#include <perf/perf_counter.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/actuator_outputs.h>
#include <uORB/topics/esc_status.h>
#include <hrtimer.h>
#include <mixer_module/mixer_module.hpp>

class UavcanEscController {
public:
    static constexpr int      MAX_ACTUATORS         = esc_status_s::CONNECTED_ESC_MAX;
    static constexpr unsigned MAX_RATE_HZ           = 400;
    static constexpr uint16_t DISARMED_OUTPUT_VALUE = UINT16_MAX;

    static_assert(uavcan::equipment::esc::RawCommand::FieldTypes::cmd::MaxSize >= MAX_ACTUATORS, "Too many actuators");

    UavcanEscController(uavcan::INode &node);
    ~UavcanEscController() = default;

    int init();

    void update_outputs(bool stop_motors, uint16_t outputs[MAX_ACTUATORS], unsigned num_outputs);

    /**
     * Sets the number of rotors and enable timer
     */
    void set_rotor_count(uint8_t count);

    static int max_output_value() {
        return uavcan::equipment::esc::RawCommand::FieldTypes::cmd::RawValueType::max();
    }

    esc_status_s &esc_status() {
        return _esc_status;
    }

private:
    /**
     * ESC status message reception will be reported via this callback.
     */
    void esc_status_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::esc::Status> &msg);

    /**
     * Checks all the ESCs freshness based on timestamp, if an ESC exceeds the timeout then is flagged offline.
     */
    uint8_t check_escs_status();

    typedef uavcan::MethodBinder<UavcanEscController *,
                                 void (UavcanEscController::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::esc::Status> &)>
        StatusCbBinder;

    typedef uavcan::MethodBinder<UavcanEscController *,
                                 void (UavcanEscController::*)(const uavcan::TimerEvent &)>
        TimerCbBinder;

    esc_status_s _esc_status{};

    uORB::PublicationMulti<esc_status_s> _esc_status_pub{ORB_ID(esc_status)};

    uint8_t _rotor_count{0};

    /*
     * libuavcan related things
     */
    uavcan::MonotonicTime                                              _prev_cmd_pub; ///< rate limiting
    uavcan::INode                                                     &_node;
    uavcan::Publisher<uavcan::equipment::esc::RawCommand>              _uavcan_pub_raw_cmd;
    uavcan::Subscriber<uavcan::equipment::esc::Status, StatusCbBinder> _uavcan_sub_status;

    /*
     * ESC states
     */
    uint8_t _max_number_of_nonzero_outputs{0};
};
