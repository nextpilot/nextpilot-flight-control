/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <rtthread.h>
#include <rtdevice.h>
#include "device_id.h"
#include "device_type.h"

namespace nextpilot::device {

class Device {
public:
    // no copy, assignment, move, move assignment
    Device(const Device &)            = delete;
    Device &operator=(const Device &) = delete;
    Device(Device &&)                 = delete;
    Device &operator=(Device &&)      = delete;

    Device() = default;

    Device(const char *devname) :
        _dev_name(devname) {
    }

    // /dev/ist8310, DRV_DEVTYPE_IST8310, DeviceBusType_SPI, 1, PH0
    Device(const char *devname, uint8_t devtype, DeviceBusType bustype, uint8_t busindex, uint8_t address) :
        _dev_name(devname) {
        // 设置device_id
        set_device_type(devtype);
        set_device_bus_type(bustype);
        set_device_bus_index(busindex);
        set_device_address(address);
    }

    // /dev/ist8310, DRV_DEVTYPE_IST8310,  spi10, PH0,
    Device(const char *devname, uint8_t devtype, const char *busname, uint8_t address) :
        _dev_name(devname),
        _bus_name(busname) {
        // 设置device_id
        set_device_type(devtype);
        set_device_address(address);
    }

    virtual ~Device() = default;

    bool valid() {
        if (_bus_device) {
            return true;
        }

        if (!_bus_name) {
            return false;
        }

        _bus_device = rt_device_find(_bus_name);
        if (_bus_device) {
            switch (_bus_device->type) {
            case RT_Device_Class_I2CBUS:
                set_device_bus_type(DeviceBusType_I2C);
                set_device_bus_index(0);
                break;
            case RT_Device_Class_SPIBUS:
                set_device_bus_type(DeviceBusType_SPI);
                set_device_bus_index(0);
            default:
                break;
            }
        }

        return _bus_device != nullptr ? true : false;
    }

    /**
	 * Initialise the driver and make it ready for use.
	 *
	 * @return	OK if the driver initialized OK, negative errno otherwise;
	 */
    virtual rt_err_t init() {
        return 0;
    }

    /**
	 * Read directly from the device.
	 *
	 * The actual size of each unit quantity is device-specific.
	 *
	 * @param offset	The device address at which to start reading
	 * @param data		The buffer into which the read values should be placed.
	 * @param count		The number of items to read.
	 * @return		The number of items read on success, negative errno otherwise.
	 */
    virtual rt_ssize_t read(rt_off_t pos, void *buffer, rt_size_t size) {
        return 0;
    }

    /**
	 * Write directly to the device.
	 *
	 * The actual size of each unit quantity is device-specific.
	 *
	 * @param address	The device address at which to start writing.
	 * @param data		The buffer from which values should be read.
	 * @param count		The number of items to write.
	 * @return		The number of items written on success, negative errno otherwise.
	 */
    virtual rt_ssize_t write(rt_off_t pos, const void *buffer, rt_size_t size) {
        return 0;
    }

    /**
	 * Read a register from the device.
	 *
	 * @param		The register to read.
	 * @return		The value that was read.
	 */
    virtual uint8_t read_reg(unsigned reg) {
        return 0;
    }

    /**
	 * Write a register in the device.
	 *
	 * @param reg		The register to write.
	 * @param value		The new value to write.
	 * @return		OK on success, negative errno otherwise.
	 */
    virtual rt_err_t write_reg(unsigned reg, uint8_t value) {
        return 0;
    }

    /**
	 * Perform a device-specific operation.
	 *
	 * @param operation	The operation to perform.
	 * @param arg		An argument to the operation.
	 * @return		Negative errno on error, OK or positive value on success.
	 */
    virtual int ioctl(int cmd, unsigned long arg) {
        return 0;
    }

    const char *get_devname() const {
        return _dev_name;
    }

    const char *get_busname() const {
        return _bus_name;
    }

    virtual bool external() const {
        return _external;
    }

    // 操作device_id
    uint32_t get_device_id() const {
        return _dev_id.devid;
    }

    /**
	 * Return the device type
	 *
	 * @return The device type
	 */
    uint8_t get_device_type() const {
        return _dev_id.devid_s.devtype;
    }

    void set_device_type(uint8_t devtype) {
        _dev_id.devid_s.devtype = devtype;
    }

    /**
	 * Return the bus type the device is connected to.
	 *
	 * @return The bus type
	 */
    DeviceBusType get_device_bus_type() const {
        // 总线类型，比如i2c
        return _dev_id.devid_s.bus_type;
    }

    void set_device_bus_type(DeviceBusType bus_type) {
        _dev_id.devid_s.bus_type = bus_type;
    }

    /**
	 * Return the bus ID the device is connected to.
	 *
	 * @return The bus ID
	 */
    uint8_t get_device_bus_index() const {
        // 总线序号，比如第1个i2c

        return _dev_id.devid_s.bus_index;
    }

    void set_device_bus_index(uint8_t bus) {
        _dev_id.devid_s.bus_index = bus;
    }

    /**
	 * Return the bus address of the device.
	 *
	 * @return The bus address
	 */
    uint8_t get_device_address() const {
        // 总线地址或者片选序号

        return _dev_id.devid_s.address;
    }

    void set_device_address(int address) {
        _dev_id.devid_s.address = address;
    }

protected:
    // 注册设备名称，比如/dev/ist8310_1
    const char *_dev_name{nullptr};

    // 注册设备id
    union DeviceId _dev_id {};

    // 通信接口名称，比如i2c1，spi10
    const char *_bus_name{nullptr};
    // 通信接口地址
    rt_device_t _bus_device{nullptr};

    // 是否外部设备（安装在飞控外部）
    bool _external{false};
};

} // namespace nextpilot::device
#endif // __DEVICE_H__
