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
 * @author Dmitry Ponomarev <ponomarevda96@gmail.com>
 */

#include "gyro.hpp"
#include <lib/drivers/gyroscope/PX4Gyroscope.hpp>

const char *const UavcanGyroBridge::NAME = "gyro";

UavcanGyroBridge::UavcanGyroBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_gyro", ORB_ID(sensor_gyro)),
    _sub_imu_data(node) {
}

int UavcanGyroBridge::init() {
    int res = _sub_imu_data.start(ImuCbBinder(this, &UavcanGyroBridge::imu_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    return 0;
}

void UavcanGyroBridge::imu_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::RawIMU> &msg) {
    uavcan_bridge::Channel *channel = get_channel_for_node(msg.getSrcNodeID().get());

    if (channel == nullptr) {
        // Something went wrong - no channel to publish on; return
        return;
    }

    // Cast our generic CDev pointer to the sensor-specific driver class
    PX4Gyroscope *gyro = (PX4Gyroscope *)channel->h_driver;

    if (gyro == nullptr) {
        return;
    }

    gyro->update(hrt_absolute_time(),
                 msg.rate_gyro_latest[0],
                 msg.rate_gyro_latest[1],
                 msg.rate_gyro_latest[2]);
}

int UavcanGyroBridge::init_driver(uavcan_bridge::Channel *channel) {
    // update device id as we now know our device node_id
    DeviceId device_id{_device_id};

    device_id.devid_s.devtype = DRV_GYR_DEVTYPE_UAVCAN;
    device_id.devid_s.address = static_cast<uint8_t>(channel->node_id);

    channel->h_driver = new PX4Gyroscope(device_id.devid);

    if (channel->h_driver == nullptr) {
        return PX4_ERROR;
    }

    PX4Gyroscope *gyro = (PX4Gyroscope *)channel->h_driver;

    channel->instance = gyro->get_instance();

    if (channel->instance < 0) {
        PX4_ERR("UavcanGyro: Unable to get a class instance");
        delete gyro;
        channel->h_driver = nullptr;
        return PX4_ERROR;
    }

    return PX4_OK;
}
