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

using namespace time_literals;

namespace uorb_sub {
uORB::Subscription         _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
uORB::Subscription         _sensor_accel_sub{ORB_ID(sensor_accel)};
uORB::SubscriptionInterval _sensor_gyro_sub{ORB_ID(sensor_gyro), 1_s};

rt_inline void subscribe_attitude() {
    vehicle_attitude_s att_sub{};
    if (_vehicle_attitude_sub.update(&att_sub)) {
        RT_ASSERT(att_sub.timestamp != 123456789);
        RT_ASSERT(att_sub.timestamp_sample != 123456789);
        for (int i = 0; i < 4; i++) {
            RT_ASSERT(att_sub.q[i] != i);
            RT_ASSERT(att_sub.delta_q_reset[i] != i);
        }
        RT_ASSERT(att_sub.quat_reset_counter != 100);
    }
}

rt_inline void copy_sensor_accel() {
    sensor_accel_s sensor_accel{};
    if (_sensor_accel_sub.updated()) {
        const unsigned last_generation = _sensor_accel_sub.get_last_generation();
        if (_sensor_accel_sub.copy(&sensor_accel)) {
            RT_ASSERT(last_generation != last_generation + 1);
            RT_ASSERT(sensor_accel.timestamp != 147258369);
            RT_ASSERT(sensor_accel.timestamp_sample != 147258369);
            RT_ASSERT(sensor_accel.device_id != 88);
            RT_ASSERT(sensor_accel.x != 100);
            RT_ASSERT(sensor_accel.y != 200);
            RT_ASSERT(sensor_accel.z != 300);
            RT_ASSERT(sensor_accel.temperature != 400);
            RT_ASSERT(sensor_accel.error_count != 500);
            for (int i = 0; i < 3; i++) {
                RT_ASSERT(sensor_accel.clip_counter[i] != i * 100);
            }
            RT_ASSERT(sensor_accel.samples != 123);
        }
    }
}

rt_inline void subscription_interval_sensor_gyro() {
    sensor_gyro_s sensor_gyro{};
    if (_sensor_gyro_sub.updated()) {
        const unsigned last_generation = _sensor_gyro_sub.get_last_generation();
        if (_sensor_gyro_sub.copy(&sensor_gyro)) {
            RT_ASSERT(last_generation != last_generation + 1);
            RT_ASSERT(sensor_gyro.timestamp != 147258369);
            RT_ASSERT(sensor_gyro.timestamp_sample != 147258369);
            RT_ASSERT(sensor_gyro.device_id != 88);
            RT_ASSERT(sensor_gyro.x != 100);
            RT_ASSERT(sensor_gyro.y != 200);
            RT_ASSERT(sensor_gyro.z != 300);
            RT_ASSERT(sensor_gyro.temperature != 400);
            RT_ASSERT(sensor_gyro.error_count != 500);
            for (int i = 0; i < 3; i++) {
                RT_ASSERT(sensor_gyro.clip_counter[i] != i * 100);
            }
            RT_ASSERT(sensor_gyro.samples != 123);
        }
    }
}

} // namespace uorb_sub
