/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef COMMAND_LONG_HPP
#define COMMAND_LONG_HPP

#include <uORB/topics/vehicle_command.h>

class MavlinkStreamCommandLong : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamCommandLong(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "COMMAND_LONG";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_COMMAND_LONG;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return 0; // commands stream is not regular and not predictable
    }

private:
    explicit MavlinkStreamCommandLong(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _vehicle_command_sub{ORB_ID(vehicle_command)};

    bool send() override {
        bool sent = false;

        static constexpr size_t COMMAND_LONG_SIZE       = MAVLINK_MSG_ID_COMMAND_LONG_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
        int                     vehicle_command_updates = 0;

        while ((_mavlink->get_free_tx_buf() >= COMMAND_LONG_SIZE) && _vehicle_command_sub.updated() && (vehicle_command_updates < vehicle_command_s::ORB_QUEUE_LENGTH)) {
            vehicle_command_updates++;

            const unsigned    last_generation = _vehicle_command_sub.get_last_generation();
            vehicle_command_s cmd;

            if (_vehicle_command_sub.update(&cmd)) {
                if (_vehicle_command_sub.get_last_generation() != last_generation + 1) {
                    PX4_ERR("COMMAND_LONG vehicle_command lost, generation %d -> %d", last_generation,
                            _vehicle_command_sub.get_last_generation());
                }

                // mavlink mavlink commands are <= UINT16_MAX
                const bool px4_internal_cmd = (cmd.command >= vehicle_command_s::VEHICLE_CMD_PX4_INTERNAL_START);

                // internal commands
                const bool target_system_internal = (cmd.target_system == _mavlink->get_system_id()) && (cmd.target_component == _mavlink->get_component_id()) && (cmd.source_system == cmd.target_system) && (cmd.source_component == cmd.target_component);

                if (!cmd.from_external && !px4_internal_cmd && !target_system_internal) {
                    PX4_DEBUG("sending command %ld to %d/%d", cmd.command, cmd.target_system, cmd.target_component);

                    MavlinkCommandSender::instance().handle_vehicle_command(cmd, _mavlink->get_channel());
                    sent = true;

                } else {
                    PX4_DEBUG("not forwarding command %ld to %d/%d", cmd.command, cmd.target_system, cmd.target_component);
                }
            }
        }

        MavlinkCommandSender::instance().check_timeout(_mavlink->get_channel());

        return sent;
    }
};

#endif // COMMAND_LONG_HPP
