/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "mavlink_command_sender.h"
#include <ulog/log.h>

#define CMD_DEBUG(FMT, ...) PX4_LOG_NAMED_COND("cmd sender", _debug_enabled, FMT, ##__VA_ARGS__)

MavlinkCommandSender *MavlinkCommandSender::_instance = nullptr;
struct rt_semaphore   MavlinkCommandSender::_lock;

void MavlinkCommandSender::initialize() {
    if (_instance == nullptr) {
        rt_sem_init(&_lock, "mav_cmd_lock", 1, RT_IPC_FLAG_PRIO);

        _instance = new MavlinkCommandSender();
    }
}

MavlinkCommandSender &MavlinkCommandSender::instance() {
    return *_instance;
}

MavlinkCommandSender::~MavlinkCommandSender() {
    rt_sem_detach(&_lock);
}

int MavlinkCommandSender::handle_vehicle_command(const vehicle_command_s &command, mavlink_channel_t channel) {
    // commands > uint16 are PX4 internal only
    if (command.command >= vehicle_command_s::VEHICLE_CMD_PX4_INTERNAL_START) {
        return 0;
    }

    lock();
    CMD_DEBUG("new command: %" PRIu32 " (channel: %d)", command.command, channel);

    mavlink_command_long_t msg = {};
    msg.target_system          = command.target_system;
    msg.target_component       = command.target_component;
    msg.command                = command.command;
    msg.confirmation           = command.confirmation;
    msg.param1                 = command.param1;
    msg.param2                 = command.param2;
    msg.param3                 = command.param3;
    msg.param4                 = command.param4;
    msg.param5                 = command.param5;
    msg.param6                 = command.param6;
    msg.param7                 = command.param7;
    mavlink_msg_command_long_send_struct(channel, &msg);

    bool already_existing = false;
    _commands.reset_to_start();

    while (command_item_s *item = _commands.get_next()) {
        if (item->timestamp_us == command.timestamp) {
            // We should activate the channel by setting num_sent_per_channel from -1 to 0.
            item->num_sent_per_channel[channel] = 0;
            already_existing                    = true;
            break;
        }
    }

    if (!already_existing) {
        command_item_s new_item;
        new_item.command                       = msg;
        new_item.timestamp_us                  = command.timestamp;
        new_item.num_sent_per_channel[channel] = 0;
        new_item.last_time_sent_us             = hrt_absolute_time();
        _commands.put(new_item);
    }

    unlock();
    return 0;
}

void MavlinkCommandSender::handle_mavlink_command_ack(const mavlink_command_ack_t &ack,
                                                      uint8_t from_sysid, uint8_t from_compid, uint8_t channel) {
    CMD_DEBUG("handling result %" PRIu8 " for command %" PRIu16 " (from %" PRIu8 ":%" PRIu8 ")",
              ack.result, ack.command, from_sysid, from_compid);
    lock();

    _commands.reset_to_start();

    while (command_item_s *item = _commands.get_next()) {
        // Check if the incoming ack matches any of the commands that we have sent.
        if (item->command.command == ack.command
            && (item->command.target_system == 0
                || from_sysid == item->command.target_system)
            && (item->command.target_component == 0
                || from_compid == item->command.target_component)
            && item->num_sent_per_channel[channel] != -1) {
            item->num_sent_per_channel[channel] = -2; // mark this as acknowledged
            break;
        }
    }

    unlock();
}

void MavlinkCommandSender::check_timeout(mavlink_channel_t channel) {
    lock();

    _commands.reset_to_start();

    while (command_item_s *item = _commands.get_next()) {
        if (hrt_elapsed_time(&item->last_time_sent_us) <= TIMEOUT_US) {
            // We keep waiting for the timeout.
            continue;
        }

        // Loop through num_sent_per_channel and check if any channel has receives an ack for this command
        // (indicated by the value -2). We avoid removing the command at the time of receiving the ack
        // as some channels might be lagging behind and will end up putting the same command into the buffer.
        bool dropped_command = false;

        for (unsigned i = 0; i < MAVLINK_COMM_NUM_BUFFERS; ++i) {
            if (item->num_sent_per_channel[i] == -2) {
                _commands.drop_current();
                dropped_command = true;
                break;
            }
        }

        if (dropped_command) {
            continue;
        }

        // The goal of this is to retry from all channels. Therefore, we keep
        // track of the retry count for each channel.
        //
        // When the first channel does a retry, the timeout is reset.
        // (e.g. all channel have done 2 retries, then channel 0 is called
        // and does retry number 3, and also resets the timeout timestamp).

        // First, we need to determine what the current max and min retry level
        // are because we can only level up, if all have caught up.
        // If num_sent_per_channel is at -1, the channel is inactive.
        int8_t max_sent = 0;
        int8_t min_sent = INT8_MAX;

        for (unsigned i = 0; i < MAVLINK_COMM_NUM_BUFFERS; ++i) {
            if (item->num_sent_per_channel[i] > max_sent) {
                max_sent = item->num_sent_per_channel[i];
            }

            if ((item->num_sent_per_channel[i] != -1)
                && (item->num_sent_per_channel[i] < min_sent)) {
                min_sent = item->num_sent_per_channel[i];
            }
        }

        if (item->num_sent_per_channel[channel] < max_sent && item->num_sent_per_channel[channel] != -1) {
            // We are behind and need to do a retransmission.
            item->command.confirmation = ++item->num_sent_per_channel[channel];
            mavlink_msg_command_long_send_struct(channel, &item->command);

            CMD_DEBUG("command %" PRIu16 " sent (not first, retries: %" PRIu8 "/%" PRIi8 ", channel: %d)",
                      item->command.command,
                      item->num_sent_per_channel[channel],
                      max_sent,
                      channel);

        } else if (item->num_sent_per_channel[channel] == max_sent
                   && min_sent == max_sent) {
            // If the next retry would be above the needed retries anyway, we can
            // drop the item, and continue with other items.
            if (item->num_sent_per_channel[channel] + 1 > RETRIES) {
                CMD_DEBUG("command %" PRIu16 " dropped", item->command.command);
                _commands.drop_current();
                continue;
            }

            // We are the first of a new retransmission series.
            item->command.confirmation = ++item->num_sent_per_channel[channel];
            mavlink_msg_command_long_send_struct(channel, &item->command);
            // Therefore, we are the ones setting the timestamp of this retry round.
            item->last_time_sent_us = hrt_absolute_time();

            CMD_DEBUG("command %" PRIu16 " sent (first, retries: %" PRId8 "/%" PRId8 ", channel: %d)",
                      item->command.command,
                      item->num_sent_per_channel[channel],
                      max_sent,
                      channel);

        } else {
            // We are already ahead, so this should not happen.
            // If it ever does, just ignore it. It will timeout eventually.
            continue;
        }
    }

    unlock();
}
