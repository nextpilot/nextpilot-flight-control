/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file CDev.cpp
 *
 * Character device base class.
 */

#include "CDev.hpp"
#include <cstring>
#include <ulog/log.h>

namespace cdev {

static rt_err_t dev_init(rt_device_t dev) {
    CDev *obj = (CDev *)dev;
    return obj->init();
}

static rt_err_t dev_open(rt_device_t dev, rt_uint16_t oflag) {
    CDev *obj = (CDev *)dev;
    return obj->open(oflag);
}

static rt_err_t dev_close(rt_device_t dev) {
    CDev *obj = (CDev *)dev;
    return obj->close();
}

static rt_ssize_t dev_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) {
    CDev *obj = (CDev *)dev;
    return obj->read(pos, buffer, size);
}

static rt_ssize_t dev_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
    CDev *obj = (CDev *)dev;
    return obj->write(pos, buffer, size);
}

static rt_err_t dev_control(rt_device_t dev, int cmd, void *args) {
    CDev *obj = (CDev *)dev;
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

CDev::CDev(const char *devname) :
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
    PX4_DEBUG("CDev::CDev");

    int ret = rt_sem_init(&_lock, "cdev_lock", 1, RT_IPC_FLAG_PRIO);

    if (ret != 0) {
        PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
    }
}

CDev::~CDev() {
    PX4_DEBUG("CDev::~CDev");

    if (_registered) {
        rt_device_unregister(this);
    }

    // if (_pollset) {
    //     delete[] (_pollset);
    // }

    rt_sem_detach(&_lock);
}

int CDev::register_class_devname(const char *class_devname) {
    PX4_DEBUG("CDev::register_class_devname %s", class_devname);

    if (class_devname == nullptr) {
        return -EINVAL;
    }

    int class_instance = 0;
    int ret            = -ENOSPC;

    while (class_instance < 4) {
        char name[32];
        rt_snprintf(name, sizeof(name), "%s%d", class_devname, class_instance);
        ret = rt_device_register(this, name, 0);

        if (ret == OK) {
            break;
        }

        class_instance++;
    }

    if (class_instance == 4) {
        return ret;
    }

    return class_instance;
}

int CDev::unregister_class_devname(const char *class_devname, unsigned class_instance) {
    PX4_DEBUG("CDev::unregister_class_devname");

    // char name[32];
    // rt_snprintf(name, sizeof(name), "%s%u", class_devname, class_instance);
    return rt_device_unregister(this);
}

int CDev::init() {
    PX4_DEBUG("CDev::init");

    int ret = PX4_OK;

    // now register the driver
    if (_devname != nullptr) {
        ret = rt_device_register(this, _devname, 0);

        if (ret == PX4_OK) {
            _registered = true;
        }

    } else {
        ret = -ENODEV;
    }

    return ret;
}

/*
 * Default implementations of the character device interface
 */
int CDev::open(rt_uint16_t oflag) {
    PX4_DEBUG("CDev::open");
    int ret = PX4_OK;

    lock();
    /* increment the open count */
    _open_count++;

    if (_open_count == 1) {
        /* first-open callback may decline the open */
        ret = open_first(oflag);

        if (ret != PX4_OK) {
            _open_count--;
        }
    }

    unlock();

    return ret;
}

int CDev::close() {
    PX4_DEBUG("CDev::close");
    int ret = PX4_OK;

    lock();

    if (_open_count > 0) {
        /* decrement the open count */
        _open_count--;

        /* callback cannot decline the close */
        if (_open_count == 0) {
            ret = close_last();
        }

    } else {
        ret = -EBADF;
    }

    unlock();

    return ret;
}

// int CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, bool setup) {
//     PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
//     int ret = PX4_OK;

//     if (setup) {
//         /*
//          * Save the file pointer in the pollfd for the subclass'
//          * benefit.
//          */
//         fds->priv = (void *)filep;
//         PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

//         /*
//          * Lock against poll_notify() and possibly other callers (protect _pollset).
//          */
//         ATOMIC_ENTER;

//         /*
//          * Try to store the fds for later use and handle array resizing.
//          */
//         while ((ret = store_poll_waiter(fds)) == -ENFILE) {
//             // No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

//             if (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
//                 ret = -ENOMEM;
//                 break;
//             }

//             const uint8_t         new_count    = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
//             px4_pollfd_struct_t **prev_pollset = _pollset;

// #ifdef __PX4_NUTTX
//             // malloc uses a semaphore, we need to call it enabled IRQ's
//             px4_leave_critical_section(flags);
// #endif
//             px4_pollfd_struct_t **new_pollset = new px4_pollfd_struct_t *[new_count];

// #ifdef __PX4_NUTTX
//             flags = px4_enter_critical_section();
// #endif

//             if (prev_pollset == _pollset) {
//                 // no one else updated the _pollset meanwhile, so we're good to go
//                 if (!new_pollset) {
//                     ret = -ENOMEM;
//                     break;
//                 }

//                 if (_max_pollwaiters > 0) {
//                     memset(new_pollset + _max_pollwaiters, 0, sizeof(px4_pollfd_struct_t *) * (new_count - _max_pollwaiters));
//                     memcpy(new_pollset, _pollset, sizeof(px4_pollfd_struct_t *) * _max_pollwaiters);
//                 }

//                 _pollset                   = new_pollset;
//                 _pollset[_max_pollwaiters] = fds;
//                 _max_pollwaiters           = new_count;

//                 // free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
// #ifdef __PX4_NUTTX
//                 px4_leave_critical_section(flags);
// #endif

//                 if (prev_pollset) {
//                     delete[] (prev_pollset);
//                 }

// #ifdef __PX4_NUTTX
//                 flags = px4_enter_critical_section();
// #endif

//                 // Success
//                 ret = PX4_OK;
//                 break;
//             }

// #ifdef __PX4_NUTTX
//             px4_leave_critical_section(flags);
// #endif
//             // We have to retry
//             delete[] new_pollset;
// #ifdef __PX4_NUTTX
//             flags = px4_enter_critical_section();
// #endif
//         }

//         if (ret == PX4_OK) {
//             /*
//              * Check to see whether we should send a poll notification
//              * immediately.
//              */
//             fds->revents |= fds->events & poll_state(filep);

//             /* yes? post the notification */
//             if (fds->revents != 0) {
//                 rt_sem_release(fds->sem);
//             }
//         }

//         ATOMIC_LEAVE;

//     } else {
//         ATOMIC_ENTER;
//         /*
//          * Handle a teardown request.
//          */
//         ret = remove_poll_waiter(fds);
//         ATOMIC_LEAVE;
//     }

//     return ret;
// }

// void CDev::poll_notify(px4_pollevent_t events) {
//     PX4_DEBUG("CDev::poll_notify events = %0x", events);

//     /* lock against poll() as well as other wakeups */
//     ATOMIC_ENTER;

//     for (unsigned i = 0; i < _max_pollwaiters; i++) {
//         if (nullptr != _pollset[i]) {
//             poll_notify_one(_pollset[i], events);
//         }
//     }

//     ATOMIC_LEAVE;
// }

// void CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events) {
//     PX4_DEBUG("CDev::poll_notify_one");

//     /* update the reported event set */
//     fds->revents |= fds->events & events;

//     PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

//     if (fds->revents != 0) {
//         rt_sem_release(fds->sem);
//     }
// }

// int CDev::store_poll_waiter(px4_pollfd_struct_t *fds) {
//     // Look for a free slot.
//     PX4_DEBUG("CDev::store_poll_waiter");

//     for (unsigned i = 0; i < _max_pollwaiters; i++) {
//         if (nullptr == _pollset[i]) {
//             /* save the pollfd */
//             _pollset[i] = fds;

//             return PX4_OK;
//         }
//     }

//     return -ENFILE;
// }

// int CDev::remove_poll_waiter(px4_pollfd_struct_t *fds) {
//     PX4_DEBUG("CDev::remove_poll_waiter");

//     for (unsigned i = 0; i < _max_pollwaiters; i++) {
//         if (fds == _pollset[i]) {
//             _pollset[i] = nullptr;
//             return PX4_OK;
//         }
//     }

//     PX4_DEBUG("poll: bad fd state");
//     return -EINVAL;
// }

} // namespace cdev
