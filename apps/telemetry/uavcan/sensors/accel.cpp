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

#include "accel.hpp"
#include <lib/drivers/accelerometer/PX4Accelerometer.hpp>

const char *const UavcanAccelBridge::NAME = "accel";

UavcanAccelBridge::UavcanAccelBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_accel", ORB_ID(sensor_accel)),
    _sub_imu_data(node) {
}

int UavcanAccelBridge::init() {
    int res = _sub_imu_data.start(ImuCbBinder(this, &UavcanAccelBridge::imu_sub_cb));

    if (res < 0) {
        DEVICE_LOG("failed to start uavcan sub: %d", res);
        return res;
    }

    return 0;
}

void UavcanAccelBridge::imu_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::RawIMU> &msg) {
    uavcan_bridge::Channel *channel = get_channel_for_node(msg.getSrcNodeID().get());

    const hrt_abstime timestamp_sample = hrt_absolute_time();

    if (channel == nullptr) {
        // Something went wrong - no channel to publish on; return
        return;
    }

    // Cast our generic CDev pointer to the sensor-specific driver class
    PX4Accelerometer *accel = (PX4Accelerometer *)channel->h_driver;

    if (accel == nullptr) {
        return;
    }

    accel->set_error_count(0);
    accel->update(timestamp_sample, msg.accelerometer_latest[0], msg.accelerometer_latest[1], msg.accelerometer_latest[2]);
}

int UavcanAccelBridge::init_driver(uavcan_bridge::Channel *channel) {
    // update device id as we now know our device node_id
    DeviceId device_id{_device_id};

    device_id.devid_s.devtype = DRV_ACC_DEVTYPE_UAVCAN;
    device_id.devid_s.address = static_cast<uint8_t>(channel->node_id);

    channel->h_driver = new PX4Accelerometer(device_id.devid);

    if (channel->h_driver == nullptr) {
        return PX4_ERROR;
    }

    PX4Accelerometer *accel = (PX4Accelerometer *)channel->h_driver;

    channel->instance = accel->get_instance();

    if (channel->instance < 0) {
        PX4_ERR("UavcanAccel: Unable to get a class instance");
        delete accel;
        channel->h_driver = nullptr;
        return PX4_ERROR;
    }

    return PX4_OK;
}
