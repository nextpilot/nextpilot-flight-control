/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "device-new.hpp"

static rt_err_t dev_init(rt_device_t dev) {
    DeviceBase *obj = (DeviceBase *)dev;
    return obj->init();
}

static rt_err_t dev_open(rt_device_t dev, rt_uint16_t oflag) {
    DeviceBase *obj = (DeviceBase *)dev;
    return obj->open(oflag);
}

static rt_err_t dev_close(rt_device_t dev) {
    DeviceBase *obj = (DeviceBase *)dev;
    return obj->close();
}

static rt_ssize_t dev_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) {
    DeviceBase *obj = (DeviceBase *)dev;
    return obj->read(pos, buffer, size);
}

static rt_ssize_t dev_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
    DeviceBase *obj = (DeviceBase *)dev;
    return obj->write(pos, buffer, size);
}

static rt_err_t dev_control(rt_device_t dev, int cmd, void *args) {
    DeviceBase *obj = (DeviceBase *)dev;
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

DeviceBase::DeviceBase(const char *devname) :
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
    },
    _devname(devname) {
    int ret = rt_sem_init(&_lock, _devname, 0, RT_IPC_FLAG_PRIO);
    if (ret != 0) {
        LOG_E("init sem fail of %s", _devname);
    }
}

DeviceBase::~DeviceBase() {
    if (_registered) {
        rt_device_unregister((rt_device_t)this);
    }

    // if (_pollset) {
    //     delete[] (_pollset);
    // }

    rt_sem_detach(&_lock);
}

int DeviceBase::register_class_devname(const char *class_devname) {
    if (!class_devname) {
        return -1;
    }

    int class_instance = 0;
    int ret            = -1;

    while (class_instance < 4) {
        char name[32];
        rt_snprintf(name, sizeof(name), "%s%d", class_devname, class_instance);
        ret = rt_device_register(this, name, 0);
        if (ret == RT_EOK) {
            break;
        }
        class_instance++;
    }

    if (class_instance == 4) {
        return ret;
    }

    return class_instance;
}

rt_err_t DeviceBase::unregister_class_devname(const char *class_devname, unsigned class_instance) {
    char name[32];
    rt_snprintf(name, sizeof(name), "%s%u", class_devname, class_instance);
    return rt_device_unregister(this);
}

rt_err_t DeviceBase::register_device() {
    if (!_devname) {
        return -1;
    }

    return rt_device_register(this, _devname, 0);
}

// int reg_a_dev() {
//     static struct rt_device _mydev;
//     return rt_device_register(&_mydev, "/obj/s/ds/we/g/we/gw/we", 0);
// }
// INIT_APP_EXPORT(reg_a_dev);
