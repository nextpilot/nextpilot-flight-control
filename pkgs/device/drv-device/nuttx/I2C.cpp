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
 * @file I2C.cpp
 *
 * Base class for devices attached via the I2C bus.
 *
 * @todo Bus frequency changes; currently we do nothing with the value
 *       that is supplied.  Should we just depend on the bus knowing?
 */

#include "I2C.hpp"

#if defined(CONFIG_I2C)

#include <px4_platform_common/i2c_spi_buses.h>
#include <nuttx/i2c/i2c_master.h>

namespace device
{
/*
 *  N.B. By defaulting the value of _bus_clocks to non Zero
 *  All calls to init() will NOT set the buss frequency
 */

unsigned int I2C::_bus_clocks[PX4_NUMBER_I2C_BUSES] = PX4_I2C_BUS_CLOCK_INIT;

I2C::I2C(uint8_t device_type, const char *name, const int bus, const uint16_t address, const uint32_t frequency) :
	CDev(name, nullptr),
	_frequency(frequency)
{
	// fill in _device_id fields for a I2C device
	_device_id.devid_s.devtype = device_type;
	_device_id.devid_s.bus_type = DeviceBusType_I2C;
	_device_id.devid_s.bus = bus;
	_device_id.devid_s.address = address;
}

I2C::I2C(const I2CSPIDriverConfig &config)
	: I2C(config.devid_driver_index, config.module_name, config.bus, config.i2c_address, config.bus_frequency)
{
}

I2C::~I2C()
{
	if (_dev) {
		px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
	}
}

int
I2C::set_bus_clock(unsigned bus, unsigned clock_hz)
{
	int index = bus - 1;

	if (index < 0 || index >= static_cast<int>(sizeof(_bus_clocks) / sizeof(_bus_clocks[0]))) {
		return -EINVAL;
	}

	if (_bus_clocks[index] > 0) {
		// DEVICE_DEBUG("overriding clock of %u with %u Hz\n", _bus_clocks[index], clock_hz);
	}

	_bus_clocks[index] = clock_hz;

	return OK;
}

int
I2C::init()
{
	int ret = PX4_ERROR;
	unsigned bus_index;

	// attach to the i2c bus
	_dev = px4_i2cbus_initialize(get_device_bus());

	if (_dev == nullptr) {
		DEVICE_DEBUG("failed to init I2C");
		ret = -ENOENT;
		goto out;
	}

	// the above call fails for a non-existing bus index,
	// so the index math here is safe.
	bus_index = get_device_bus() - 1;

	// abort if the max frequency we allow (the frequency we ask)
	// is smaller than the bus frequency
	if (_bus_clocks[bus_index] > _frequency) {
		(void)px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
		DEVICE_LOG("FAIL: too slow for bus #%u: %u KHz, device max: %" PRIu32 " KHz)",
			   get_device_bus(), _bus_clocks[bus_index] / 1000, _frequency / 1000);
		ret = -EINVAL;
		goto out;
	}

	// set frequency for this instance once to the bus speed
	// the bus speed is the maximum supported by all devices on the bus,
	// as we have to prioritize performance over compatibility.
	// If a new device requires a lower clock speed, this has to be
	// manually set via "fmu i2c <bus> <clock>" before starting any
	// drivers.
	// This is necessary as automatically lowering the bus speed
	// for maximum compatibility could induce timing issues on
	// critical sensors the adopter might be unaware of.

	// set the bus frequency on the first access if it has
	// not been set yet
	if (_bus_clocks[bus_index] == 0) {
		_bus_clocks[bus_index] = _frequency;
	}

	// call the probe function to check whether the device is present
	ret = probe();

	if (ret != OK) {
		DEVICE_DEBUG("probe failed");
		goto out;
	}

	// do base class init, which will create device node, etc
	ret = CDev::init();

	if (ret != OK) {
		DEVICE_DEBUG("cdev init failed");
		goto out;
	}

	// tell the world where we are
	DEVICE_DEBUG("on I2C bus %d at 0x%02x (bus: %u KHz, max: %" PRIu32 " KHz)",
		     get_device_bus(), get_device_address(), _bus_clocks[bus_index] / 1000, _frequency / 1000);

out:

	if ((ret != OK) && (_dev != nullptr)) {
		px4_i2cbus_uninitialize(_dev);
		_dev = nullptr;
	}

	return ret;
}

int
I2C::transfer(const uint8_t *send, const unsigned send_len, uint8_t *recv, const unsigned recv_len)
{
	int ret = PX4_ERROR;
	unsigned retry_count = 0;

	if (_dev == nullptr) {
		PX4_ERR("I2C device not opened");
		return PX4_ERROR;
	}

	do {
		DEVICE_DEBUG("transfer out %p/%u  in %p/%u", send, send_len, recv, recv_len);

		i2c_msg_s msgv[2] {};
		unsigned msgs = 0;

		if (send_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = 0;
			msgv[msgs].buffer = const_cast<uint8_t *>(send);
			msgv[msgs].length = send_len;
			msgs++;
		}

		if (recv_len > 0) {
			msgv[msgs].frequency = _bus_clocks[get_device_bus() - 1];
			msgv[msgs].addr = get_device_address();
			msgv[msgs].flags = I2C_M_READ;
			msgv[msgs].buffer = recv;
			msgv[msgs].length = recv_len;
			msgs++;
		}

		if (msgs == 0) {
			return -EINVAL;
		}

		int ret_transfer = I2C_TRANSFER(_dev, &msgv[0], msgs);

		if (ret_transfer != 0) {
			DEVICE_DEBUG("I2C transfer failed, result %d", ret_transfer);
			ret = PX4_ERROR;

		} else {
			// success
			ret = PX4_OK;
			break;
		}

		// if we have already retried once, and we aren't going to give up, then reset the bus
		if ((_retries > 0) && (retry_count < _retries)) {
#if defined(CONFIG_I2C_RESET)
			DEVICE_DEBUG("I2C bus: %d, Addr: %X, I2C_RESET %d/%d",
				     get_device_bus(), get_device_address(), retry_count + 1, _retries);
			I2C_RESET(_dev);
#endif // CONFIG_I2C_RESET
		}

	} while (retry_count++ < _retries);

	return ret;
}

} // namespace device

#endif // CONFIG_I2C
