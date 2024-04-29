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
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#include "mag.hpp"

#include <hrtimer.h>
#include <systemlib/err.h>

#include <magnetometer/PX4Magnetometer.hpp>

const char *const UavcanMagnetometerBridge::NAME = "mag";

UavcanMagnetometerBridge::UavcanMagnetometerBridge(uavcan::INode &node) :
    UavcanSensorBridgeBase("uavcan_mag", ORB_ID(sensor_mag)),
    _sub_mag(node),
    _sub_mag2(node) {
}

int UavcanMagnetometerBridge::init() {
    int res = _sub_mag.start(MagCbBinder(this, &UavcanMagnetometerBridge::mag_sub_cb));

    if (res < 0) {
        PX4_ERR("failed to start uavcan sub: %d", res);
        return res;
    }

    int res2 = _sub_mag2.start(Mag2CbBinder(this, &UavcanMagnetometerBridge::mag2_sub_cb));

    if (res2 < 0) {
        PX4_ERR("failed to start uavcan sub2: %d", res2);
        return res2;
    }

    return 0;
}

void UavcanMagnetometerBridge::mag_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::MagneticFieldStrength> &msg) {
    uavcan_bridge::Channel *channel = get_channel_for_node(msg.getSrcNodeID().get());

    if (channel == nullptr) {
        // Something went wrong - no channel to publish on; return
        return;
    }

    // Cast our generic CDev pointer to the sensor-specific driver class
    PX4Magnetometer *mag = (PX4Magnetometer *)channel->h_driver;

    if (mag == nullptr) {
        return;
    }

    const float x = msg.magnetic_field_ga[0];
    const float y = msg.magnetic_field_ga[1];
    const float z = msg.magnetic_field_ga[2];

    mag->update(hrt_absolute_time(), x, y, z);
}

void UavcanMagnetometerBridge::mag2_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::ahrs::MagneticFieldStrength2> &msg) {
    uavcan_bridge::Channel *channel = get_channel_for_node(msg.getSrcNodeID().get());

    if (channel == nullptr || channel->instance < 0) {
        // Something went wrong - no channel to publish on; return
        return;
    }

    // Cast our generic CDev pointer to the sensor-specific driver class
    PX4Magnetometer *mag = (PX4Magnetometer *)channel->h_driver;

    if (mag == nullptr) {
        return;
    }

    const float x = msg.magnetic_field_ga[0];
    const float y = msg.magnetic_field_ga[1];
    const float z = msg.magnetic_field_ga[2];

    mag->update(hrt_absolute_time(), x, y, z);
}

int UavcanMagnetometerBridge::init_driver(uavcan_bridge::Channel *channel) {
    // update device id as we now know our device node_id
    DeviceId device_id{_device_id};

    device_id.devid_s.devtype = DRV_MAG_DEVTYPE_UAVCAN;
    device_id.devid_s.address = static_cast<uint8_t>(channel->node_id);

    channel->h_driver = new PX4Magnetometer(device_id.devid, ROTATION_NONE);

    if (channel->h_driver == nullptr) {
        return PX4_ERROR;
    }

    PX4Magnetometer *mag = (PX4Magnetometer *)channel->h_driver;

    channel->instance = mag->get_instance();

    if (channel->instance < 0) {
        PX4_ERR("UavcanMag: Unable to get an instance");
        delete mag;
        channel->h_driver = nullptr;
        return PX4_ERROR;
    }

    return PX4_OK;
}
