/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef STATUSTEXT_HPP
#define STATUSTEXT_HPP

#include <uORB/topics/mavlink_log.h>

#include <lib/perf/perf_counter.h>

class MavlinkStreamStatustext : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamStatustext(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "STATUSTEXT";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_STATUSTEXT;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _mavlink_log_sub.updated() ? (MAVLINK_MSG_ID_STATUSTEXT_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES) : 0;
    }

private:
    explicit MavlinkStreamStatustext(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    ~MavlinkStreamStatustext() {
        perf_free(_missed_msg_count_perf);
    }

    uORB::Subscription _mavlink_log_sub{ORB_ID(mavlink_log)};
    perf_counter_t     _missed_msg_count_perf{perf_alloc(PC_COUNT, MODULE_NAME ": STATUSTEXT missed messages")};
    uint16_t           _id{0};

    bool send() override {
        if (_mavlink->is_gcs_connected()) {
            while (_mavlink_log_sub.updated() && (_mavlink->get_free_tx_buf() >= get_size())) {
                const unsigned last_generation = _mavlink_log_sub.get_last_generation();

                mavlink_log_s mavlink_log;

                if (_mavlink_log_sub.update(&mavlink_log)) {
                    // don't send stale messages
                    if (hrt_elapsed_time(&mavlink_log.timestamp) < 5_s) {
                        if (_mavlink_log_sub.get_last_generation() != (last_generation + 1)) {
                            perf_count(_missed_msg_count_perf);
                            PX4_DEBUG("channel %d has missed %lld mavlink log messages", _mavlink->get_channel(),
                                      perf_event_count(_missed_msg_count_perf));
                        }

                        mavlink_statustext_t msg{};
                        const char          *text           = mavlink_log.text;
                        constexpr unsigned   max_chunk_size = sizeof(msg.text);
                        msg.severity                        = mavlink_log.severity;
                        msg.chunk_seq                       = 0;
                        msg.id                              = _id++;
                        unsigned text_size;

                        while ((text_size = strlen(text)) > 0) {
                            unsigned chunk_size = math::min(text_size, max_chunk_size);

                            if (chunk_size < max_chunk_size) {
                                memcpy(&msg.text[0], &text[0], chunk_size);
                                // pad with zeros
                                memset(&msg.text[0] + chunk_size, 0, max_chunk_size - chunk_size);

                            } else {
                                memcpy(&msg.text[0], &text[0], chunk_size);
                            }

                            mavlink_msg_statustext_send_struct(_mavlink->get_channel(), &msg);

                            if (text_size <= max_chunk_size) {
                                break;

                            } else {
                                text += max_chunk_size;
                            }

                            msg.chunk_seq += 1;
                        }

                        return true;
                    }
                }
            }
        }

        return false;
    }
};

#endif // STATUSTEXT_HPP
