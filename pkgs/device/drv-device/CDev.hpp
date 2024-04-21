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
 * @file CDev.hpp
 *
 * Definitions for the generic base classes in the device framework.
 */

#ifndef _DEVICE_CDEV_HPP
#define _DEVICE_CDEV_HPP

#include "Device.hpp"
#include <cdev/CDev.hpp>

//#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/posix.h>

/**
 * Namespace encapsulating all device framework classes, functions and data.
 */
namespace device {

using file_t = cdev::file_t;

/**
 * Abstract class for any character device
 */
class __EXPORT CDev : public Device, public cdev::CDev {
public:
    /**
	 * Constructor
	 *
	 * @param name		Driver name
	 * @param devname	Device node name
	 */
    CDev(const char *name, const char *devname); // TODO: dagar remove name and Device inheritance

    virtual ~CDev() = default;

    virtual int init();

    /**
	 * Perform an ioctl operation on the device.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @param cmd		The ioctl command value.
	 * @param arg		The ioctl argument value.
	 * @return		OK on success, or -errno otherwise.
	 */
    virtual int ioctl(file_t *filep, int cmd, unsigned long arg) {
        return -ENOTTY;
    }
};

} // namespace device

// class instance for primary driver of each class
enum CLASS_DEVICE {
    CLASS_DEVICE_PRIMARY   = 0,
    CLASS_DEVICE_SECONDARY = 1,
    CLASS_DEVICE_TERTIARY  = 2
};

#endif /* _DEVICE_CDEV_HPP */
