/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __SPI_H__
#define __SPI_H__

#include "device.hpp"

typedef uint32_t spi_drdy_gpio_t;

typedef struct {
    uint32_t        cs_gpio;   ///< chip-select GPIO (0 if this device is not used)
    spi_drdy_gpio_t drdy_gpio; ///< data ready GPIO (0 if not set)
    uint32_t        devid;     ///< SPIDEV_ID(type,index). For PX4 devices on NuttX: index is the device type, and for external buses the CS index
    uint16_t        devtype;   ///< driver device type, e.g. DRV_IMU_DEVTYPE_ICM20689 (on NuttX: PX4_SPI_DEV_ID(devid) == devtype_driver)

} spi_bus_device_t;

namespace nextpilot::device {

class SPI : public Device {
    SPI(int16_t chipselect = -1, int bus = -1);
};
} // namespace nextpilot::device


#endif // __SPI_H__
