/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __DEVICE_DEVID_H__
#define __DEVICE_DEVID_H__

#include "device_type.h"
#include <stdint.h>

namespace nextpilot::device {

// class instance for primary driver of each class
enum CLASS_DEVICE {
    CLASS_DEVICE_PRIMARY   = 0,
    CLASS_DEVICE_SECONDARY = 1,
    CLASS_DEVICE_TERTIARY  = 2
};

/** DeviceBus bus types for DEVID */
enum DeviceBusType {
    DeviceBusType_UNKNOWN    = 0,
    DeviceBusType_I2C        = 1,
    DeviceBusType_SPI        = 2,
    DeviceBusType_UAVCAN     = 3,
    DeviceBusType_SIMULATION = 4,
    DeviceBusType_SERIAL     = 5,
    DeviceBusType_MAVLINK    = 6,
};

/*
  broken out device elements. The bitfields are used to keep
  the overall value small enough to fit in a float accurately,
  which makes it possible to transport over the MAVLink
  parameter protocol without loss of information.
 */
struct DeviceStructure {
    // 总线类型
    DeviceBusType bus_type : 3;
    // 总线序号
    uint8_t bus_index : 5; // which instance of the bus type
    // i2c地址或者片选序号
    union {
        uint8_t address;  // address on the bus (eg. I2C address)
        uint8_t selector; // chip select number
    };
    uint8_t devtype; // device class specific device type
};

union DeviceId {
    struct DeviceStructure devid_s;
    struct {
        DeviceBusType bustype : 3;
        uint8_t       busidx : 5;
        uint8_t       address;
        uint8_t       devtype;
    };
    uint32_t devid{0};
};

static const char *get_device_bus_string(DeviceBusType bus) {
    switch (bus) {
    case DeviceBusType_I2C:
        return "I2C";

    case DeviceBusType_SPI:
        return "SPI";

    case DeviceBusType_UAVCAN:
        return "UAVCAN";

    case DeviceBusType_SIMULATION:
        return "SIMULATION";

    case DeviceBusType_SERIAL:
        return "SERIAL";

    case DeviceBusType_MAVLINK:
        return "MAVLINK";

    case DeviceBusType_UNKNOWN:
    default:
        return "UNKNOWN";
    }
}

/**
 * Print decoded device id string to a buffer.
 *
 * @param buffer                        buffer to write to
 * @param length                        buffer length
 * @param id	                        The device id.
 * @param return                        number of bytes written
 */
static inline int device_id_print_buffer(char *buffer, int length, uint32_t id) {
    DeviceId dev_id{};
    dev_id.devid = id;

    int num_written = rt_snprintf(buffer, length, "Type: 0x%02X, %s:%d (0x%02X)", dev_id.devid_s.devtype,
                                  get_device_bus_string(dev_id.devid_s.bus_type), dev_id.devid_s.bus_index, dev_id.devid_s.address);

    buffer[length - 1] = 0; // ensure 0-termination

    return num_written;
}

} // namespace nextpilot::device

#endif // __DEVICE_DEVID_H__
