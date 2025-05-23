/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <hrtimer.h>
#include <defines.h>


#include <param/param.h>

// for ekf2 replay
#include <uORB/topics/airspeed.h>
#include <uORB/topics/distance_sensor.h>
#include <uORB/topics/landing_target_pose.h>
#include <uORB/topics/sensor_combined.h>
#include <uORB/topics/vehicle_air_data.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/vehicle_land_detected.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_magnetometer.h>
#include <uORB/topics/vehicle_optical_flow.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/vehicle_odometry.h>

#include "ReplayEkf2.hpp"

namespace nextpilot::replay {

bool ReplayEkf2::handleTopicUpdate(Subscription &sub, void *data, std::ifstream &replay_file) {
    if (sub.orb_meta == ORB_ID(ekf2_timestamps)) {
        ekf2_timestamps_s ekf2_timestamps;
        memcpy(&ekf2_timestamps, data, sub.orb_meta->o_size);

        if (!publishEkf2Topics(ekf2_timestamps, replay_file)) {
            return false;
        }

        // Wait for modules to process the data
        px4_lockstep_wait_for_components();

        return true;

    } else if (sub.orb_meta == ORB_ID(vehicle_status) || sub.orb_meta == ORB_ID(vehicle_land_detected) || sub.orb_meta == ORB_ID(vehicle_gps_position)) {
        return publishTopic(sub, data);
    } // else: do not publish

    return false;
}

void ReplayEkf2::onSubscriptionAdded(Subscription &sub, uint16_t msg_id) {
    if (sub.orb_meta == ORB_ID(sensor_combined)) {
        _sensor_combined_msg_id = msg_id;

    } else if (sub.orb_meta == ORB_ID(airspeed)) {
        _airspeed_msg_id = msg_id;

    } else if (sub.orb_meta == ORB_ID(distance_sensor)) {
        _distance_sensor_msg_id = msg_id;

    } else if (sub.orb_meta == ORB_ID(vehicle_optical_flow)) {
        _optical_flow_msg_id = msg_id;

    } else if (sub.orb_meta == ORB_ID(vehicle_air_data)) {
        _vehicle_air_data_msg_id = msg_id;

    } else if (sub.orb_meta == ORB_ID(vehicle_magnetometer)) {
        _vehicle_magnetometer_msg_id = msg_id;

    } else if (sub.orb_meta == ORB_ID(vehicle_visual_odometry)) {
        _vehicle_visual_odometry_msg_id = msg_id;
    }

    // the main loop should only handle publication of the following topics, the sensor topics are
    // handled separately in publishEkf2Topics()
    // Note: the GPS is not treated here since not missing data is more important than the accuracy of the timestamp
    sub.ignored = sub.orb_meta != ORB_ID(ekf2_timestamps) && sub.orb_meta != ORB_ID(vehicle_status) && sub.orb_meta != ORB_ID(vehicle_land_detected) && sub.orb_meta != ORB_ID(vehicle_gps_position);
}

bool ReplayEkf2::publishEkf2Topics(const ekf2_timestamps_s &ekf2_timestamps, std::ifstream &replay_file) {
    auto handle_sensor_publication = [&](int16_t timestamp_relative, uint16_t msg_id) {
        if (timestamp_relative != ekf2_timestamps_s::RELATIVE_TIMESTAMP_INVALID) {
            // timestamp_relative is already given in 0.1 ms
            uint64_t t = timestamp_relative + ekf2_timestamps.timestamp / 100; // in 0.1 ms
            findTimestampAndPublish(t, msg_id, replay_file);
        }
    };

    handle_sensor_publication(ekf2_timestamps.airspeed_timestamp_rel, _airspeed_msg_id);
    handle_sensor_publication(ekf2_timestamps.distance_sensor_timestamp_rel, _distance_sensor_msg_id);
    handle_sensor_publication(ekf2_timestamps.optical_flow_timestamp_rel, _optical_flow_msg_id);
    handle_sensor_publication(ekf2_timestamps.vehicle_air_data_timestamp_rel, _vehicle_air_data_msg_id);
    handle_sensor_publication(ekf2_timestamps.vehicle_magnetometer_timestamp_rel, _vehicle_magnetometer_msg_id);
    handle_sensor_publication(ekf2_timestamps.visual_odometry_timestamp_rel, _vehicle_visual_odometry_msg_id);

    // sensor_combined: publish last because ekf2 is polling on this
    if (!findTimestampAndPublish(ekf2_timestamps.timestamp / 100, _sensor_combined_msg_id, replay_file)) {
        if (_sensor_combined_msg_id == msg_id_invalid) {
            // subscription not found yet or sensor_combined not contained in log
            return false;

        } else if (!_subscriptions[_sensor_combined_msg_id]->orb_meta) {
            return false; // read past end of file

        } else {
            // we should publish a topic, just publish the same again
            readTopicDataToBuffer(*_subscriptions[_sensor_combined_msg_id], replay_file);
            publishTopic(*_subscriptions[_sensor_combined_msg_id], _read_buffer.data());
        }
    }

    return true;
}

bool ReplayEkf2::findTimestampAndPublish(uint64_t timestamp, uint16_t msg_id, std::ifstream &replay_file) {
    if (msg_id == msg_id_invalid) {
        // could happen if a topic is not logged
        return false;
    }

    Subscription &sub = *_subscriptions[msg_id];

    while (sub.next_timestamp / 100 < timestamp && sub.orb_meta) {
        nextDataMessage(replay_file, sub, msg_id);
    }

    if (!sub.orb_meta) { // no messages anymore
        return false;
    }

    if (sub.next_timestamp / 100 != timestamp) {
        // this can happen in beginning of the log or on a dropout
        PX4_DEBUG("No timestamp match found for topic %s (%i, %i)", sub.orb_meta->o_name, (int)sub.next_timestamp / 100,
                  timestamp);
        ++sub.error_counter;
        return false;
    }

    readTopicDataToBuffer(sub, replay_file);
    publishTopic(sub, _read_buffer.data());
    return true;
}

void ReplayEkf2::onEnterMainLoop() {
    _speed_factor = 0.f; // iterate as fast as possible

    // disable parameter auto save
    param_control_autosave(false);
}

void ReplayEkf2::onExitMainLoop() {
    // print statistics
    auto print_sensor_statistics = [this](uint16_t msg_id, const char *name) {
        if (msg_id != msg_id_invalid) {
            Subscription &sub = *_subscriptions[msg_id];

            if (sub.publication_counter > 0 || sub.error_counter > 0) {
                PX4_INFO("%s: %i, %i", name, sub.publication_counter, sub.error_counter);
            }
        }
    };

    PX4_INFO("");
    PX4_INFO("Topic, Num Published, Num Error (no timestamp match found):");

    print_sensor_statistics(_airspeed_msg_id, "airspeed");
    print_sensor_statistics(_distance_sensor_msg_id, "distance_sensor");
    print_sensor_statistics(_optical_flow_msg_id, "vehicle_optical_flow");
    print_sensor_statistics(_sensor_combined_msg_id, "sensor_combined");
    print_sensor_statistics(_vehicle_air_data_msg_id, "vehicle_air_data");
    print_sensor_statistics(_vehicle_magnetometer_msg_id, "vehicle_magnetometer");
    print_sensor_statistics(_vehicle_visual_odometry_msg_id, "vehicle_visual_odometry");
}

} // namespace nextpilot::replay
