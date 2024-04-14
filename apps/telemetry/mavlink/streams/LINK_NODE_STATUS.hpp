/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef LINK_NODE_STATUS_HPP
#define LINK_NODE_STATUS_HPP

class MavlinkStreamLinkNodeStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamLinkNodeStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "LINK_NODE_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_LINK_NODE_STATUS;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_LINK_NODE_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamLinkNodeStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    bool send() override {
        if (_mavlink->get_free_tx_buf() >= get_size()) {
            mavlink_link_node_status_t link_node_status{};

            const telemetry_status_s &tstatus  = _mavlink->telemetry_status();
            link_node_status.tx_buf            = 0; // % TODO
            link_node_status.rx_buf            = 0; // % TODO
            link_node_status.tx_rate           = tstatus.tx_rate_avg;
            link_node_status.rx_rate           = tstatus.rx_rate_avg;
            link_node_status.rx_parse_err      = tstatus.rx_parse_errors;
            link_node_status.tx_overflows      = tstatus.tx_buffer_overruns;
            link_node_status.rx_overflows      = tstatus.rx_buffer_overruns;
            link_node_status.messages_sent     = tstatus.tx_message_count;
            link_node_status.messages_received = tstatus.rx_message_count;
            link_node_status.messages_lost     = tstatus.rx_message_lost_count;

            link_node_status.timestamp = hrt_absolute_time();

            mavlink_msg_link_node_status_send_struct(_mavlink->get_channel(), &link_node_status);

            return true;
        }

        return false;
    }
};

#endif // LINK_NODE_STATUS_HPP
