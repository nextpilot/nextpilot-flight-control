/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __RT_DEVICE_H__
#define __RT_DEVICE_H__

#include <rtthread.h>
#include <rtdevice.h>

class RTDevice;

static rt_err_t _dev_init(rt_device_t dev) {
    RTDevice *obj = (RTDevice *)dev;
    return obj->init();
}

static rt_err_t _dev_open(rt_device_t dev, rt_uint16_t oflag) {
    RTDevice *obj = (RTDevice *)dev;
    return obj->open(oflag);
}

static rt_err_t _dev_close(rt_device_t dev) {
    RTDevice *obj = (RTDevice *)dev;
    return obj->close();
}

static rt_ssize_t _dev_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) {
    RTDevice *obj = (RTDevice *)dev;
    return obj->read(pos, buffer, size);
}

static rt_ssize_t _dev_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
    RTDevice *obj = (RTDevice *)dev;
    return obj->write(pos, buffer, size);
}

static rt_err_t _dev_control(rt_device_t dev, int cmd, void *args) {
    RTDevice *obj = (RTDevice *)dev;
    return obj->control(cmd, args);
}

static rt_err_t _dev_register(rt_device_t dev) {
}

#ifdef RT_USING_DEVICE_OPS
const struct rt_device_ops _dev_ops = {
    .init    = _dev_init,
    .open    = _dev_open,
    .close   = _dev_close,
    .read    = _dev_read,
    .write   = _dev_write,
    .control = _dev_control,
};
#endif // RT_USING_DEVICE_OPS

class RTDevice : public rt_device {
public:
    RTDevice() {
    }

    RTDevice(const char *devname) :
        _devname(devname) {
    }

    rt_err_t _init_() {
        type        = RT_Device_Class_Char;
        rx_indicate = nullptr;
        tx_complete = nullptr;
#ifdef RT_USING_DEVICE_OPS
        ops = &_dev_ops;
#else
        init    = _dev_init;
        open    = _dev_open;
        close   = _dev_close;
        read    = _dev_read;
        write   = _dev_write;
        control = _dev_control;
#endif // RT_USING_DEVICE_OPS
        fops      = nullptr;
        user_data = nullptr;
    }

    virtual ~RTDevice() {
        if (_registered) {
            device_unregister();
        }
    }

    virtual rt_err_t init() {
        return RT_EOK;
    }

    virtual rt_err_t open(rt_uint16_t oflag) {
        return RT_EOK;
    }

    virtual rt_err_t close() {
        return RT_EOK;
    }

    rt_ssize_t read(rt_off_t pos, void *buffer, rt_size_t size) {
        return 0;
    }

    rt_ssize_t write(rt_off_t pos, const void *buffer, rt_size_t size) {
        return 0;
    }

    rt_err_t control(int cmd, void *arg) {
        return RT_EOK;
    }

    rt_err_t device_register(const char *name, rt_uint16_t flags) {
        _registered = true;
        return rt_device_register(this, name, flags);
    }

    rt_err_t device_unregister() {
        _registered = false;
        return rt_device_unregister(this);
    }

private:
    const char *_devname{nullptr};
    bool        _registered{false};
};

#endif // __RT_DEVICE_H__
