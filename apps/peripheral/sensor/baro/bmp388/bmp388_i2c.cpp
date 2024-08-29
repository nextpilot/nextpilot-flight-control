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
 * @file bmp388_i2c.cpp
 *
 * I2C interface for BMP388
 */

#include <drivers/device/i2c.h>

#include "bmp388.h"

class BMP388_I2C : public device::I2C, public IBMP388 {
public:
    BMP388_I2C(uint8_t bus, uint32_t device, int bus_frequency);
    virtual ~BMP388_I2C() = default;

    int init();

    uint8_t        get_reg(uint8_t addr);
    int            get_reg_buf(uint8_t addr, uint8_t *buf, uint8_t len);
    int            set_reg(uint8_t value, uint8_t addr);
    calibration_s *get_calibration(uint8_t addr);

    uint32_t get_device_id() const override {
        return device::I2C::get_device_id();
    }

    uint8_t get_device_address() const override {
        return device::I2C::get_device_address();
    }

    void set_device_type(uint8_t devtype);

private:
    struct calibration_s _cal;
};

IBMP388 *bmp388_i2c_interface(uint8_t busnum, uint32_t device, int bus_frequency) {
    return new BMP388_I2C(busnum, device, bus_frequency);
}

BMP388_I2C::BMP388_I2C(uint8_t bus, uint32_t device, int bus_frequency) :
    I2C(DRV_BARO_DEVTYPE_BMP388, MODULE_NAME, bus, device, bus_frequency) {
}

int BMP388_I2C::init() {
    return I2C::init();
}

uint8_t BMP388_I2C::get_reg(uint8_t addr) {
    uint8_t cmd[2] = {(uint8_t)(addr), 0};
    transfer(&cmd[0], 1, &cmd[1], 1);

    return cmd[1];
}

int BMP388_I2C::get_reg_buf(uint8_t addr, uint8_t *buf, uint8_t len) {
    const uint8_t cmd = (uint8_t)(addr);
    return transfer(&cmd, sizeof(cmd), buf, len);
}

int BMP388_I2C::set_reg(uint8_t value, uint8_t addr) {
    uint8_t cmd[2] = {(uint8_t)(addr), value};
    return transfer(cmd, sizeof(cmd), nullptr, 0);
}

calibration_s *BMP388_I2C::get_calibration(uint8_t addr) {
    const uint8_t cmd = (uint8_t)(addr);

    if (transfer(&cmd, sizeof(cmd), (uint8_t *)&_cal, sizeof(struct calibration_s)) == OK) {
        return &(_cal);

    } else {
        return nullptr;
    }
}

void BMP388_I2C::set_device_type(uint8_t devtype) {
    device::Device::set_device_type(devtype);
}
