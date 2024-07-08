/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
#define LOG_TAG "vconsole"
#define LOG_LVL LOG_LVL_INFO

#include <rtdbg.h>
#include <string.h>
#include "vconsole.h"

// 动态切换shell端口，参考下面的链接
// https://gitee.com/RT-Thread-Mirror/vconsole/blob/main/vconsole.c

#ifdef RT_USING_POSIX_STDIO

#   include <ipc/waitqueue.h>
#   include <sys/unistd.h>
#   include <sys/ioctl.h>
#   include <fcntl.h>
#   include <poll.h>
#   include <dfs_file.h>

int rt_posix_stdio_get_console(void);
int rt_posix_stdio_set_console(const char *device_name, int mode);
int sys_dup2(int oldfd, int new);

static rt_err_t _fops_rx_ind(rt_device_t dev, rt_size_t size) {
    rt_wqueue_wakeup(&(dev->wait_queue), (void *)POLLIN);
    return RT_EOK;
}

static int _fops_open(struct dfs_file *fd) {
    rt_uint16_t flags = 0;

    rt_device_t device = (rt_device_t)fd->vnode->data;
    RT_ASSERT(device != RT_NULL);

    switch (fd->flags & O_ACCMODE) {
    case O_RDONLY:
        LOG_D("fops open: O_RDONLY!");
        flags = RT_DEVICE_FLAG_RDONLY;
        break;
    case O_WRONLY:
        LOG_D("fops open: O_WRONLY!");
        flags = RT_DEVICE_FLAG_WRONLY;
        break;
    case O_RDWR:
        LOG_D("fops open: O_RDWR!");
        flags = RT_DEVICE_FLAG_RDWR;
        break;
    default:
        LOG_E("fops open: unknown mode - %d!", fd->flags & O_ACCMODE);
        break;
    }

    if ((fd->flags & O_ACCMODE) != O_WRONLY) {
        rt_device_set_rx_indicate(device, _fops_rx_ind);
    }

    return rt_device_open(device, flags);
}

static int _fops_close(struct dfs_file *fd) {
    rt_device_t device = (rt_device_t)fd->vnode->data;
    RT_ASSERT(device != RT_NULL);
    rt_device_set_rx_indicate(device, RT_NULL);
    rt_device_close(device);

    return 0;
}

static int _fops_ioctl(struct dfs_file *fd, int cmd, void *args) {
    rt_device_t device = (rt_device_t)fd->vnode->data;
    RT_ASSERT(device != RT_NULL);
    switch (cmd) {
    case FIONREAD:
        break;
    case FIONWRITE:
        break;
    }

    return rt_device_control(device, cmd, args);
}

#   ifdef RT_USING_DFS_V1
// ssize_t (*read)(struct dfs_file *fd, void *buf, size_t count)
static ssize_t _fops_read(struct dfs_file *fd, void *buf, size_t count)
#   else
//    ssize_t (*read)(struct dfs_file *file, void *buf, size_t count, off_t *pos)
static ssize_t _fops_read(struct dfs_file *fd, void *buf, size_t count, off_t *pos)
#   endif // RT_USING_DFS_V1
{
    ssize_t     size   = 0;
    rt_device_t device = (rt_device_t)fd->vnode->data;
    RT_ASSERT(device != RT_NULL);
    do {
        size = rt_device_read(device, -1, buf, count);
        if (size <= 0) {
            if (fd->flags & O_NONBLOCK) {
                size = -EAGAIN;
                break;
            }

            rt_wqueue_wait(&(device->wait_queue), 0, RT_WAITING_FOREVER);
        }
    } while (size <= 0);

    return size;
}
#   ifdef RT_USING_DFS_V1
// ssize_t (*write)    (struct dfs_file *fd, const void *buf, size_t count)
static ssize_t _fops_write(struct dfs_file *fd, const void *buf, size_t count)
#   else
// ssize_t (*write)(struct dfs_file *file, const void *buf, size_t count, off_t *pos)
static ssize_t _fops_write(struct dfs_file *fd, const void *buf, size_t count, off_t *pos)
#   endif // RT_USING_DFS_V1
{
    rt_device_t device = (rt_device_t)fd->vnode->data;
    RT_ASSERT(device != RT_NULL);
    return rt_device_write(device, -1, buf, count);
}

static int _fops_poll(struct dfs_file *fd, struct rt_pollreq *req) {
    int mask  = 0;
    int flags = 0;

    rt_device_t device = (rt_device_t)fd->vnode->data;
    RT_ASSERT(device != RT_NULL);

    rt_vconsole_device_t vcom = (rt_vconsole_device_t)device;

    /* only support POLLIN */
    flags = fd->flags & O_ACCMODE;
    if (flags == O_RDONLY || flags == O_RDWR) {
        rt_poll_add(&(device->wait_queue), req);
        if (rt_ringbuffer_data_len(vcom->rx_buff) > 0) {
            mask |= POLLIN;
        }
    }

    return mask;
}

const static struct dfs_file_ops _vconsole_fops =
    {
        .open  = _fops_open,
        .close = _fops_close,
        .ioctl = _fops_ioctl,
        .read  = _fops_read,
        .write = _fops_write,
        .flush = RT_NULL,       /* flush */
        .lseek = RT_NULL,       /* lseek */
#   ifdef RT_USING_DFS_V2
        .truncate = RT_NULL,    /* truncate */
#   endif
        .getdents = RT_NULL,    /* getdents */
        .poll     = _fops_poll, /* poll */
};

#endif // RT_USING_POSIX_STDIO

static rt_err_t _init(rt_device_t dev) {
    return RT_EOK;
}

static rt_err_t _open(rt_device_t dev, rt_uint16_t oflag) {
    return RT_EOK;
}

static rt_err_t _close(rt_device_t dev) {
    return RT_EOK;
}

static rt_ssize_t _read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size) {
    rt_vconsole_device_t vcom = (rt_vconsole_device_t)dev;
    rt_ssize_t           len  = 0;

    if (size > 0) {
        rt_enter_critical();
        len = rt_ringbuffer_get(vcom->rx_buff, (rt_uint8_t *)buffer, size);
        rt_exit_critical();
    }

    return len;
}

static rt_ssize_t _write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size) {
    rt_vconsole_device_t vcom = (rt_vconsole_device_t)dev;
    rt_ssize_t           len  = 0;

    if (size > 0) {
        rt_enter_critical();
        len = rt_ringbuffer_put_force(vcom->tx_buff, buffer, size);
        rt_exit_critical();
    }

    return len;
}

static rt_err_t _control(rt_device_t dev, int cmd, void *args) {
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops _vconsole_ops = {
    .init    = _init,
    .open    = _open,
    .close   = _close,
    .read    = _read,
    .write   = _write,
    .control = _control,
};
#endif // RT_USING_DEVICE_OPS

struct rt_vconsole_device *vconsole_create(const char *name, rt_size_t rx_buf_size, rt_size_t tx_buf_size, rt_uint8_t flag) {
    // 创建device
    rt_vconsole_device_t vcom = (rt_vconsole_device_t)rt_calloc(1, sizeof(struct rt_vconsole_device));
    if (!vcom) {
        return RT_NULL;
    }

    vcom->magic = VCONSOLE_MAGIC_NUM;
    // 创建buff
    vcom->rx_buff = rt_ringbuffer_create(rx_buf_size);
    vcom->tx_buff = rt_ringbuffer_create(tx_buf_size);

    if (!vcom->rx_buff || !vcom->tx_buff) {
        goto __exit;
    }

    // 初始化device
    rt_device_t device  = &(vcom->parent);
    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;
#ifdef RT_USING_DEVICE_OPS
    device->ops = &_vconsole_ops;
#else
    device->init    = _init;
    device->open    = _open;
    device->close   = _close;
    device->read    = _read;
    device->write   = _write;
    device->control = _control;
#endif // RT_USING_DEVICE_OPS
    device->user_data = RT_NULL;

    // 注册设备
    // RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX
    flag |= (RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    if (rt_device_register(device, name, flag) != RT_EOK) {
        goto __exit;
    }

    // 设置文件操作函数
#ifdef RT_USING_POSIX_STDIO
    /* set fops */
    device->fops = &_vconsole_fops;
#endif // RT_USING_POSIX_STDIO

    return vcom;

__exit:
    if (vcom->rx_buff) {
        rt_ringbuffer_destroy(vcom->rx_buff);
    }

    if (vcom->tx_buff) {
        rt_ringbuffer_destroy(vcom->tx_buff);
    }

    if (vcom) {
        rt_free(vcom);
    }

    return RT_NULL;
}

rt_err_t vconsole_delete(rt_vconsole_device_t vcom) {
    if (!vcom) {
        return -RT_EINVAL;
    }

    if (vcom->magic != VCONSOLE_MAGIC_NUM) {
        return -RT_ERROR;
    }

    if (&(vcom->parent) == rt_console_get_device()) {
        return -RT_EBUSY;
    }

    // 从device列表移除
    rt_device_unregister(&(vcom->parent));

    // 释放ringbuffer
    if (vcom->rx_buff) {
        rt_ringbuffer_destroy(vcom->rx_buff);
    }

    if (vcom->tx_buff) {
        rt_ringbuffer_destroy(vcom->tx_buff);
    }

    // 释放device
    rt_free(vcom);

    return RT_EOK;
}

rt_size_t vconsole_input(rt_vconsole_device_t vcom, rt_uint8_t *buffer, rt_size_t size) {
    rt_size_t len = 0;

    if (size > 0) {
        rt_enter_critical();
        len = rt_ringbuffer_put_force(vcom->rx_buff, buffer, size);
        rt_exit_critical();
        // 如果写入数据则触发rx_indicate
        if (len > 0 && vcom->parent.rx_indicate) {
            vcom->parent.rx_indicate(&vcom->parent, rt_ringbuffer_data_len(vcom->rx_buff));
        }
    }

    return len;
}

rt_size_t vconsole_output(rt_vconsole_device_t vcom, rt_uint8_t *buffer, rt_size_t size) {
    rt_size_t len = 0;

    if (size > 0) {
        rt_enter_critical();
        len = rt_ringbuffer_get(vcom->tx_buff, buffer, size);
        rt_exit_critical();
    }

    return len;
}

rt_device_t rt_console_change_device(const char *name) {
    rt_device_t new_device = rt_device_find(name);
    if (!new_device) {
        LOG_W("can not find %s\n", name);
        return RT_NULL;
    }

    // 获取老的shell设备
    rt_device_t old_device = rt_console_get_device();
    if (old_device == new_device) {
        return RT_NULL;
    }

    // 设置新的console设备（rt_kprintf输出）
    old_device = rt_console_set_device(name);

    // 设置新的shell设备
#ifdef RT_USING_POSIX_STDIO
    // 将新设备设置为STDIO
    int new_fd = rt_posix_stdio_set_console(name, O_RDWR);
    /* set fd (0, 1, 2) */
    sys_dup2(new_fd, 0);
    sys_dup2(new_fd, 1);
    sys_dup2(new_fd, 2);

    // 由于STDIO的read默认是阻塞的，需要取消阻塞
    int old_fd = rt_posix_stdio_get_console();
    int flags  = ioctl(old_fd, F_GETFL, 0);
    ioctl(old_fd, F_SETFL, (void *)(flags | O_NONBLOCK));
    // 唤醒老设备，不要继续等待了
    rt_wqueue_wakeup(&(old_device->wait_queue), (void *)POLLERR);

    // 延时下
    rt_thread_mdelay(20);
#else
    // finsh_set_device会自动关闭老设备
    finsh_set_device(new_device->parent.name);
#endif // RT_USING_POSIX_STDIO

    return old_device;
}
