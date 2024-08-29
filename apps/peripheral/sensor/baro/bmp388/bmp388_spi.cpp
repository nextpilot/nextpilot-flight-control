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
 * @file bmp388_spi.cpp
 *
 * SPI interface for BMP388 (NOTE: untested!)
 */

#include <drivers/device/spi.h>

#include "bmp388.h"


/* SPI protocol address bits */
#define DIR_READ  (1 << 7)  //for set
#define DIR_WRITE ~(1 << 7) //for clear

#pragma pack(push, 1)

struct spi_data_s {
    uint8_t       addr;
    struct data_s data;
};

struct spi_calibration_s {
    uint8_t              addr;
    struct calibration_s cal;
};

#pragma pack(pop)

class BMP388_SPI : public device::SPI, public IBMP388 {
public:
    BMP388_SPI(uint8_t bus, uint32_t device, int bus_frequency, spi_mode_e spi_mode);
    virtual ~BMP388_SPI() = default;

    int init();

    uint8_t        get_reg(uint8_t addr);
    int            get_reg_buf(uint8_t addr, uint8_t *buf, uint8_t len);
    int            set_reg(uint8_t value, uint8_t addr);
    calibration_s *get_calibration(uint8_t addr);

    uint32_t get_device_id() const override {
        return device::SPI::get_device_id();
    }

    uint8_t get_device_address() const override {
        return device::SPI::get_device_address();
    }

    void set_device_type(uint8_t devtype);

private:
    spi_calibration_s _cal;
};

IBMP388 *bmp388_spi_interface(uint8_t busnum, uint32_t device, int bus_frequency, spi_mode_e spi_mode) {
    return new BMP388_SPI(busnum, device, bus_frequency, spi_mode);
}

BMP388_SPI::BMP388_SPI(uint8_t bus, uint32_t device, int bus_frequency, spi_mode_e spi_mode) :
    SPI(DRV_BARO_DEVTYPE_BMP388, MODULE_NAME, bus, device, spi_mode, bus_frequency) {
}

int BMP388_SPI::init() {
    return SPI::init();
};

uint8_t BMP388_SPI::get_reg(uint8_t addr) {
    uint8_t cmd[2] = {(uint8_t)(addr | DIR_READ), 0}; //set MSB bit
    transfer(&cmd[0], &cmd[0], 2);

    return cmd[1];
}

int BMP388_SPI::get_reg_buf(uint8_t addr, uint8_t *buf, uint8_t len) {
    uint8_t cmd[1] = {(uint8_t)(addr | DIR_READ)};
    return transfer(&cmd[0], buf, len);
}

int BMP388_SPI::set_reg(uint8_t value, uint8_t addr) {
    uint8_t cmd[2] = {(uint8_t)(addr & DIR_WRITE), value}; //clear MSB bit
    return transfer(&cmd[0], nullptr, 2);
}

calibration_s *BMP388_SPI::get_calibration(uint8_t addr) {
    _cal.addr = addr | DIR_READ;

    if (transfer((uint8_t *)&_cal, (uint8_t *)&_cal, sizeof(struct spi_calibration_s)) == OK) {
        return &(_cal.cal);

    } else {
        return nullptr;
    }
}

void BMP388_SPI::set_device_type(uint8_t devtype) {
    device::Device::set_device_type(devtype);
}
