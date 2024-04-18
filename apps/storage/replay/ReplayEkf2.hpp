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

#include "Replay.hpp"

namespace nextpilot {

/**
 * @class ReplayEkf2
 * replay specialization for Ekf2 replay
 */
class ReplayEkf2 : public Replay {
public:
protected:
    void onEnterMainLoop() override;
    void onExitMainLoop() override;

    /**
     * handle ekf2 topic publication in ekf2 replay mode
     * @param sub
     * @param data
     * @param replay_file file currently replayed (file seek position should be considered arbitrary after this call)
     * @return true if published, false otherwise
     */
    bool handleTopicUpdate(Subscription &sub, void *data, std::ifstream &replay_file) override;

    void onSubscriptionAdded(Subscription &sub, uint16_t msg_id) override;

    uint64_t getTimestampOffset() override {
        // avoid offsetting timestamps as we use them to compare against the log
        return 0;
    }

private:
    bool publishEkf2Topics(const ekf2_timestamps_s &ekf2_timestamps, std::ifstream &replay_file);

    /**
     * find the next message for a subscription that matches a given timestamp and publish it
     * @param timestamp in 0.1 ms
     * @param msg_id
     * @param replay_file file currently replayed (file seek position should be considered arbitrary after this call)
     * @return true if timestamp found and published
     */
    bool findTimestampAndPublish(uint64_t timestamp, uint16_t msg_id, std::ifstream &replay_file);

    static constexpr uint16_t msg_id_invalid = 0xffff;

    uint16_t _airspeed_msg_id                = msg_id_invalid;
    uint16_t _distance_sensor_msg_id         = msg_id_invalid;
    uint16_t _optical_flow_msg_id            = msg_id_invalid;
    uint16_t _sensor_combined_msg_id         = msg_id_invalid;
    uint16_t _vehicle_air_data_msg_id        = msg_id_invalid;
    uint16_t _vehicle_magnetometer_msg_id    = msg_id_invalid;
    uint16_t _vehicle_visual_odometry_msg_id = msg_id_invalid;
};

} // namespace nextpilot
