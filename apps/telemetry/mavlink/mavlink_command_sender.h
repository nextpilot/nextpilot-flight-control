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
 * @file mavlink_command_sender.h
 * Mavlink commands sender with support for retransmission.
 *
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include <px4_platform_common/tasks.h>
#include <px4_platform_common/sem.h>
#include <drivers/drv_hrt.h>

#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>

#include "timestamped_list.h"
#include "mavlink_bridge_header.h"

/**
 * @class MavlinkCommandSender
 */
class MavlinkCommandSender {
public:
    /**
     * initialize: call this once on startup (this function is not thread-safe!)
     */
    static void initialize();

    static MavlinkCommandSender &instance();

    /**
     * Send a command on a channel and keep it in a queue for retransmission.
     * thread-safe
     * @return 0 on success, <0 otherwise
     */
    int handle_vehicle_command(const struct vehicle_command_s &command, mavlink_channel_t channel);

    /**
     * Check timeouts to verify if an commands need retransmission.
     * thread-safe
     */
    void check_timeout(mavlink_channel_t channel);

    /**
     * Handle mavlink command_ack.
     * thread-safe
     */
    void handle_mavlink_command_ack(const mavlink_command_ack_t &ack, uint8_t from_sysid, uint8_t from_compid,
                                    uint8_t channel);

private:
    MavlinkCommandSender() = default;

    ~MavlinkCommandSender();

    static void lock() {
        do {
        } while (px4_sem_wait(&_lock) != 0);
    }

    static void unlock() {
        px4_sem_post(&_lock);
    }

    static MavlinkCommandSender *_instance;
    static px4_sem_t             _lock;

    struct command_item_s {
        mavlink_command_long_t command           = {};
        hrt_abstime            timestamp_us      = 0;
        hrt_abstime            last_time_sent_us = 0;
        // -1: channel did not request this command to be sent, -2: channel got an ack for this command
#if MAVLINK_COMM_NUM_BUFFERS == 4
        int8_t num_sent_per_channel[MAVLINK_COMM_NUM_BUFFERS]{-1, -1, -1, -1};
#elif MAVLINK_COMM_NUM_BUFFERS == 6
        int8_t num_sent_per_channel[MAVLINK_COMM_NUM_BUFFERS]{-1, -1, -1, -1, -1, -1};
#else
#error Unknown number of MAVLINK_COMM_NUM_BUFFERS
#endif
    };

    TimestampedList<command_item_s, 3> _commands{};

    bool                      _debug_enabled = false;
    static constexpr uint8_t  RETRIES        = 3;
    static constexpr uint64_t TIMEOUT_US     = 500000;

    /* do not allow copying or assigning this class */
    MavlinkCommandSender(const MavlinkCommandSender &)           = delete;
    MavlinkCommandSender operator=(const MavlinkCommandSender &) = delete;
};
