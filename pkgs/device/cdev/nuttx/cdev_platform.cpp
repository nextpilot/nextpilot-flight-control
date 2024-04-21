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
 * @file cdev_platform.cpp
 *
 * NuttX Character device functions
 */
#include <errno.h>

#include "cdev_platform.hpp"
#include "../CDev.hpp"

#include <sys/ioctl.h>

#ifdef CONFIG_DISABLE_POLL
#   error This driver is not compatible with CONFIG_DISABLE_POLL
#endif

namespace cdev {

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
static int     cdev_open(file_t *filp);
static int     cdev_close(file_t *filp);
static ssize_t cdev_read(file_t *filp, char *buffer, size_t buflen);
static ssize_t cdev_write(file_t *filp, const char *buffer, size_t buflen);
static off_t   cdev_seek(file_t *filp, off_t offset, int whence);
static int     cdev_ioctl(file_t *filp, int cmd, unsigned long arg);
static int     cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, bool setup);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
const struct file_operations cdev::CDev::fops = {
    open : cdev_open,
    close : cdev_close,
    read : cdev_read,
    write : cdev_write,
    seek : cdev_seek,
    ioctl : cdev_ioctl,
    poll : cdev_poll,
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
    unlink : nullptr
#endif
};

static int
cdev_open(file_t *filp) {
    if ((filp->f_inode->i_flags & FSNODEFLAG_DELETED) != 0) {
        return -ENODEV;
    }

    cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

    return cdev->open(filp);
}

static int
cdev_close(file_t *filp) {
    if ((filp->f_inode->i_flags & FSNODEFLAG_DELETED) != 0) {
        return -ENODEV;
    }

    cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

    return cdev->close(filp);
}

static ssize_t
cdev_read(file_t *filp, char *buffer, size_t buflen) {
    if ((filp->f_inode->i_flags & FSNODEFLAG_DELETED) != 0) {
        return -ENODEV;
    }

    cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

    return cdev->read(filp, buffer, buflen);
}

static ssize_t
cdev_write(file_t *filp, const char *buffer, size_t buflen) {
    if ((filp->f_inode->i_flags & FSNODEFLAG_DELETED) != 0) {
        return -ENODEV;
    }

    cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

    return cdev->write(filp, buffer, buflen);
}

static off_t
cdev_seek(file_t *filp, off_t offset, int whence) {
    if ((filp->f_inode->i_flags & FSNODEFLAG_DELETED) != 0) {
        return -ENODEV;
    }

    cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

    return cdev->seek(filp, offset, whence);
}

static int
cdev_ioctl(file_t *filp, int cmd, unsigned long arg) {
    if ((filp->f_inode->i_flags & FSNODEFLAG_DELETED) != 0) {
        return -ENODEV;
    }

    cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

    return cdev->ioctl(filp, cmd, arg);
}

static int
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, bool setup) {
    if ((filp->f_inode->i_flags & FSNODEFLAG_DELETED) != 0) {
        return -ENODEV;
    }

    cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

    return cdev->poll(filp, fds, setup);
}

} // namespace cdev
