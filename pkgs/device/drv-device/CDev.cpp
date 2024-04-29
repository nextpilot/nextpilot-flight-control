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
 * @file CDev.cpp
 *
 * Character device base class.
 */

#include "CDev.hpp"

#include <cstring>

#include <ulog/log.h>

namespace device {

CDev::CDev(const char *name, const char *devname) :
    Device(name),
    cdev::CDev(devname) {
}

int CDev::init() {
    PX4_DEBUG("CDev::init");

    // base class init first
    int ret = Device::init();

    if (ret != PX4_OK) {
        goto out;
    }

    // now register the driver
    if (get_devname() != nullptr) {
        ret = cdev::CDev::init();

        if (ret != PX4_OK) {
            goto out;
        }
    }

out:
    return ret;
}

} // namespace device
