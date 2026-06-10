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

namespace uorb_pub {
uORB::PublicationData<vehicle_attitude_s> _vehicle_attitude_pub{ORB_ID(vehicle_attitude)};
uORB::PublicationData<sensor_accel_s>     _sensor_accel_pub{ORB_ID(sensor_accel)};
uORB::PublicationData<sensor_gyro_s>      _sensor_gyro_pub{ORB_ID(sensor_gyro)};

rt_inline void publish_attitude() {
    vehicle_attitude_s att_pub{};
    att_pub.timestamp        = 123456789;
    att_pub.timestamp_sample = 123456789;
    for (int i = 0; i < 4; i++) {
        att_pub.q[i]             = i;
        att_pub.delta_q_reset[i] = i;
    }
    att_pub.quat_reset_counter = 100;
    _vehicle_attitude_pub.publish(att_pub);
};

rt_inline void publish_sensor_accel() {
    sensor_accel_s sensor_accel{};
    sensor_accel.timestamp        = 147258369;
    sensor_accel.timestamp_sample = 147258369;
    sensor_accel.device_id        = 88;
    sensor_accel.x                = 100;
    sensor_accel.y                = 200;
    sensor_accel.z                = 300;
    sensor_accel.temperature      = 400;
    sensor_accel.error_count      = 500;
    for (int i = 0; i < 3; i++) {
        sensor_accel.clip_counter[i] = i * 100;
    }
    sensor_accel.samples = 123;
    _sensor_accel_pub.publish(sensor_accel);
}

rt_inline void publish_sensor_gyro() {
    sensor_gyro_s sensor_gyro{};
    sensor_gyro.timestamp        = 147258369;
    sensor_gyro.timestamp_sample = 147258369;
    sensor_gyro.device_id        = 88;
    sensor_gyro.x                = 100;
    sensor_gyro.y                = 200;
    sensor_gyro.z                = 300;
    sensor_gyro.temperature      = 400;
    sensor_gyro.error_count      = 500;
    for (int i = 0; i < 3; i++) {
        sensor_gyro.clip_counter[i] = i * 100;
    }
    sensor_gyro.samples = 123;
    _sensor_gyro_pub.publish(sensor_gyro);
}

} // namespace uorb_pub
