/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "device.hpp"
#include "defines.h"

namespace nextpilot::device {

static rt_err_t dev_init(rt_device_t dev) {
    Device *obj = (Device *)dev;
    return obj->init();
}

static rt_err_t dev_open(rt_device_t dev, rt_uint16_t oflag) {
    Device *obj = (Device *)dev;
    return obj->open(oflag);
}

static rt_err_t dev_close(rt_device_t dev) {
    Device *obj = (Device *)dev;
    return obj->close();
}

static rt_ssize_t dev_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) {
    Device *obj = (Device *)dev;
    return obj->read(pos, buffer, size);
}

static rt_ssize_t dev_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
    Device *obj = (Device *)dev;
    return obj->write(pos, buffer, size);
}

static rt_err_t dev_control(rt_device_t dev, int cmd, void *args) {
    Device *obj = (Device *)dev;
    return obj->control(cmd, args);
}

#ifdef RT_USING_DEVICE_OPS
const struct rt_device_ops _dev_ops = {
    .init    = dev_init,
    .open    = dev_open,
    .close   = dev_close,
    .read    = dev_read,
    .write   = dev_write,
    .control = dev_control,
};
#endif // RT_USING_DEVICE_OPS

Device::Device() :
    rt_device{
        .type        = RT_Device_Class_Char,
        .rx_indicate = nullptr,
        .tx_complete = nullptr,
#ifdef RT_USING_DEVICE_OPS
        .ops = &_dev_ops,
#else
        .init    = dev_init,
        .open    = dev_open,
        .close   = dev_close,
        .read    = dev_read,
        .write   = dev_write,
        .control = dev_control,
#endif // RT_USING_DEVICE_OPS
        .fops      = nullptr,
        .user_data = nullptr,
    } {
}

rt_err_t Device::init() {
    // 查找bus
    if (_bus_name) {
        _bus_device = rt_device_find(_bus_name);
    }
    // 注册dev
    if (_dev_name) {
        return register_driver(_dev_name, 0);
    }
    return 0;
}

} // namespace nextpilot::device

// int aaa() {
//     Device dev;
//     return 0;
// }
