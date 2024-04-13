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

class Device : public rt_device {
public:
    Device();

    // /dev/ist8310, 01,  spi10, spi,
    Device(const char *devname, uint8_t devtype, const char *busname, DeviceBusType bustype, uint8_t busindex, uint8_t address) :
        _dev_name(devname),
        _bus_name(busname) {
        // 设置device_id
        set_device_type(devtype);
        set_device_bus_type(bustype);
        set_device_bus_index(busindex);
        set_device_address(address);
    }

    // no copy, assignment, move, move assignment
    Device(const Device &)            = delete;
    Device &operator=(const Device &) = delete;
    Device(Device &&)                 = delete;
    Device &operator=(Device &&)      = delete;

    virtual ~Device();

    virtual rt_err_t init();

    virtual rt_err_t open(rt_uint16_t oflag) {
        return 0;
    };

    virtual rt_err_t control(int cmd, void *args) {
        return 0;
    }

    virtual rt_err_t close() {
        return 0;
    };

    virtual rt_ssize_t read(rt_off_t pos, void *buffer, rt_size_t size) {
        return 0;
    };

    virtual rt_ssize_t write(rt_off_t pos, const void *buffer, rt_size_t size) {
        return 0;
    };

    virtual off_t seek(off_t offset, int whence) {
        return 0;
    };

    virtual int ioctl(int cmd, unsigned long arg) {
        return 0;
    };

    rt_err_t register_driver(const char *devname, rt_uint16_t flags) {
        rt_err_t ret = -1;
        if (devname && !_registered) {
            ret = rt_device_register(this, devname, flags);

            if (ret == RT_EOK) {
                _registered = true;
            }
        }

        return ret;
    }

    rt_err_t unregister_driver() {
        rt_err_t ret = -1;
        if (_registered) {
            ret = rt_device_unregister(this);
            if (ret == RT_EOK) {
                _registered = false;
            }
        }
        return ret;
    }

    void lock() {
    }

    void unlock() {
    }

    const char *get_devname() const {
        return _dev_name;
    }

    virtual bool external() const {
        return false;
    }

    // 操作device_id
    uint32_t get_device_id() const {
        return _dev_id.devid;
    }

    uint8_t get_device_type() const {
        return _dev_id.devid_s.devtype;
    }

    void set_device_type(uint8_t devtype) {
        _dev_id.devid_s.devtype = devtype;
    }

    // 总线类型，比如i2c
    DeviceBusType get_device_bus_type() const {
        return _dev_id.devid_s.bus_type;
    }

    void set_device_bus_type(DeviceBusType bus_type) {
        _dev_id.devid_s.bus_type = bus_type;
    }

    // 总线序号，比如第1个i2c
    uint8_t get_device_bus_index() const {
        return _dev_id.devid_s.bus_index;
    }

    void set_device_bus_index(uint8_t bus) {
        _dev_id.devid_s.bus_index = bus;
    }

    // 总线地址或者片选序号
    uint8_t get_device_address() const {
        return _dev_id.devid_s.address;
    }

    void set_device_address(int address) {
        _dev_id.devid_s.address = address;
    }

protected:
    // 注册设备的名称比如/dev/ist8310_1
    const char    *_dev_name{nullptr};
    union DeviceId _dev_id {};
    bool           _registered{false};

    // 驱动的名称，比如ist8310
    const char *_bus_name{nullptr};
    rt_device_t _bus_device{nullptr};
};

} // namespace nextpilot::device
#endif // __DEVICE_H__
