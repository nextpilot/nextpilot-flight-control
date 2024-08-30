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
 * @file ms5611_spi.cpp
 *
 * SPI interface for MS5611
 */

#include <drivers/device/spi.h>
#include "ms5611.h"

/* SPI protocol address bits */
#define DIR_READ       (1 << 7)
#define DIR_WRITE      (0 << 7)
#define ADDR_INCREMENT (1 << 6)

class MS5611_SPI : public device::SPI {
public:
    MS5611_SPI(uint8_t bus, uint32_t device, ms5611::prom_u &prom_buf, int bus_frequency, spi_mode_e spi_mode);
    virtual ~MS5611_SPI() = default;

    virtual int init();
    virtual int read(unsigned offset, void *data, unsigned count);
    virtual int ioctl(unsigned operation, unsigned &arg);

private:
    ms5611::prom_u &_prom;

    /**
	 * Send a reset command to the MS5611.
	 *
	 * This is required after any bus reset.
	 */
    int _reset();

    /**
	 * Send a measure command to the MS5611.
	 *
	 * @param addr		Which address to use for the measure operation.
	 */
    int _measure(unsigned addr);

    /**
	 * Read the MS5611 PROM
	 *
	 * @return		OK if the PROM reads successfully.
	 */
    int _read_prom();

    /**
	 * Read a 16-bit register value.
	 *
	 * @param reg		The register to read.
	 */
    uint16_t _reg16(unsigned reg);

    /**
	 * Wrapper around transfer() that prevents interrupt-context transfers
	 * from pre-empting us. The sensor may (does) share a bus with sensors
	 * that are polled from interrupt context (or we may be pre-empted)
	 * so we need to guarantee that transfers complete without interruption.
	 */
    int _transfer(uint8_t *send, uint8_t *recv, unsigned len);
};

device::Device *
MS5611_spi_interface(ms5611::prom_u &prom_buf, uint32_t devid, uint8_t busnum, int bus_frequency, spi_mode_e spi_mode) {
    return new MS5611_SPI(busnum, devid, prom_buf, bus_frequency, spi_mode);
}

MS5611_SPI::MS5611_SPI(uint8_t bus, uint32_t device, ms5611::prom_u &prom_buf, int bus_frequency, spi_mode_e spi_mode) :
    SPI(DRV_BARO_DEVTYPE_MS5611, MODULE_NAME, bus, device, spi_mode, bus_frequency),
    _prom(prom_buf) {
}

int MS5611_SPI::init() {
    int ret = SPI::init();

    if (ret != OK) {
        LOG_D("SPI init failed");
        return RT_ERROR;
    }

    // reset and read PROM (try up to 3 times)
    for (int i = 0; i < 3; i++) {
        /* send reset command */
        ret = _reset();

        if (ret != OK) {
            LOG_D("reset failed");
            continue;
        }

        /* read PROM */
        ret = _read_prom();

        if (ret == OK) {
            return RT_EOK;

        } else {
            LOG_D("prom readout failed");
            continue;
        }
    }

    return RT_ERROR;
}

int MS5611_SPI::read(unsigned offset, void *data, unsigned count) {
    union _cvt {
        uint8_t  b[4];
        uint32_t w;
    } *cvt = (_cvt *)data;

    uint8_t buf[4] = {0 | DIR_WRITE, 0, 0, 0};

    /* read the most recent measurement */
    int ret = _transfer(&buf[0], &buf[0], sizeof(buf));

    if (ret == OK) {
        /* fetch the raw value */
        cvt->b[0] = buf[3];
        cvt->b[1] = buf[2];
        cvt->b[2] = buf[1];
        cvt->b[3] = 0;

        ret = count;
    }

    return ret;
}

int MS5611_SPI::ioctl(unsigned operation, unsigned &arg) {
    int ret;

    switch (operation) {
    case IOCTL_RESET:
        ret = _reset();
        break;

    case IOCTL_MEASURE:
        ret = _measure(arg);
        break;

    default:
        ret = EINVAL;
    }

    if (ret != OK) {
        errno = ret;
        return -1;
    }

    return 0;
}

int MS5611_SPI::_reset() {
    uint8_t cmd = ADDR_RESET_CMD | DIR_WRITE;

    return _transfer(&cmd, nullptr, 1);
}

int MS5611_SPI::_measure(unsigned addr) {
    uint8_t cmd = addr | DIR_WRITE;

    return _transfer(&cmd, nullptr, 1);
}

int MS5611_SPI::_read_prom() {
    /*
	 * Wait for PROM contents to be in the device (2.8 ms) in the case we are
	 * called immediately after reset.
	 */
    px4_usleep(3000);

    /* read and convert PROM words */
    bool all_zero = true;

    for (int i = 0; i < 8; i++) {
        uint8_t cmd = (ADDR_PROM_SETUP + (i * 2));
        _prom.c[i]  = _reg16(cmd);

        if (_prom.c[i] != 0) {
            all_zero = false;
        }

        //LOG_D("prom[%u]=0x%x", (unsigned)i, (unsigned)_prom.c[i]);
    }

    /* calculate CRC and return success/failure accordingly */
    int ret = ms5611::crc4(&_prom.c[0]) ? OK : -EIO;

    if (ret != OK) {
        LOG_D("crc failed");
    }

    if (all_zero) {
        LOG_D("prom all zero");
        ret = -EIO;
    }

    return ret;
}

uint16_t
MS5611_SPI::_reg16(unsigned reg) {
    uint8_t cmd[3] = {(uint8_t)(reg | DIR_READ), 0, 0};

    _transfer(cmd, cmd, sizeof(cmd));

    return (uint16_t)(cmd[1] << 8) | cmd[2];
}

int MS5611_SPI::_transfer(uint8_t *send, uint8_t *recv, unsigned len) {
    return transfer(send, recv, len);
}
