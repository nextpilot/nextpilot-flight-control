/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <drivers/device/device.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/sensor_baro.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/i2c_spi_buses.h>
#include <px4_platform_common/px4_work_queue/WorkItemScheduled.hpp>
#include <systemlib/err.h>

#include "ms5611.h"

enum MS56XX_DEVICE_TYPES {
    MS5611_DEVICE = 5611,
    MS5607_DEVICE = 5607,
};

/* helper macro for handling report buffer indices */
#define INCREMENT(_x, _lim)           \
    do {                              \
        __typeof__(_x) _tmp = _x + 1; \
        if (_tmp >= _lim)             \
            _tmp = 0;                 \
        _x = _tmp;                    \
    } while (0)

/* helper macro for arithmetic - returns the square of the argument */
#define POW2(_x) ((_x) * (_x))

/*
 * MS5611/MS5607 internal constants and data structures.
 */
#define ADDR_CMD_CONVERT_D1_OSR256  0x40 /* write to this address to start pressure conversion */
#define ADDR_CMD_CONVERT_D1_OSR512  0x42 /* write to this address to start pressure conversion */
#define ADDR_CMD_CONVERT_D1_OSR1024 0x44 /* write to this address to start pressure conversion */
#define ADDR_CMD_CONVERT_D1_OSR2048 0x46 /* write to this address to start pressure conversion */
#define ADDR_CMD_CONVERT_D1_OSR4096 0x48 /* write to this address to start pressure conversion */
#define ADDR_CMD_CONVERT_D2_OSR256  0x50 /* write to this address to start temperature conversion */
#define ADDR_CMD_CONVERT_D2_OSR512  0x52 /* write to this address to start temperature conversion */
#define ADDR_CMD_CONVERT_D2_OSR1024 0x54 /* write to this address to start temperature conversion */
#define ADDR_CMD_CONVERT_D2_OSR2048 0x56 /* write to this address to start temperature conversion */
#define ADDR_CMD_CONVERT_D2_OSR4096 0x58 /* write to this address to start temperature conversion */

/*
  use an OSR of 1024 to reduce the self-heating effect of the
  sensor. Information from MS tells us that some individual sensors
  are quite sensitive to this effect and that reducing the OSR can
  make a big difference
 */
#define ADDR_CMD_CONVERT_D1 ADDR_CMD_CONVERT_D1_OSR1024
#define ADDR_CMD_CONVERT_D2 ADDR_CMD_CONVERT_D2_OSR1024

/*
 * Maximum internal conversion time for OSR 1024 is 2.28 ms. We set an update
 * rate of 100Hz which is be very safe not to read the ADC before the
 * conversion finished
 */
#define MS5611_CONVERSION_INTERVAL 10000 /* microseconds */
#define MS5611_MEASUREMENT_RATIO   3     /* pressure measurements per temperature measurement */

class MS5611 : public I2CSPIDriver<MS5611> {
public:
    MS5611(device::Device *interface, ms5611::prom_u &prom_buf, const I2CSPIDriverConfig &config);
    ~MS5611() override;

    static I2CSPIDriverBase *instantiate(const I2CSPIDriverConfig &config, int runtime_instance);
    static void              print_usage();

    int init();

    /**
	 * Perform a poll cycle; collect from the previous measurement
	 * and start a new one.
	 *
	 * This is the heart of the measurement state machine.  This function
	 * alternately starts a measurement, or collects the data from the
	 * previous measurement.
	 *
	 * When the interval between measurements is greater than the minimum
	 * measurement interval, a gap is inserted between collection
	 * and measurement to provide the most recent measurement possible
	 * at the next interval.
	 */
    void RunImpl();

protected:
    void print_status() override;

    uORB::PublicationMulti<sensor_baro_s> _sensor_baro_pub{ORB_ID(sensor_baro)};

    device::Device *_interface;

    ms5611::prom_s _prom;

    enum MS56XX_DEVICE_TYPES _device_type;
    bool                     _collect_phase{false};
    unsigned                 _measure_phase{false};

    /* intermediate temperature values per MS5611/MS5607 datasheet */
    int64_t _OFF{0};
    int64_t _SENS{0};

    bool _initialized{false};

    float _last_pressure{NAN};
    float _last_temperature{NAN};

    perf_counter_t _sample_perf;
    perf_counter_t _measure_perf;
    perf_counter_t _comms_errors;

    /**
	 * Initialize the automatic measurement state machine and start it.
	 *
	 * @note This function is called at open and error time.  It might make sense
	 *       to make it more aggressive about resetting the bus in case of errors.
	 */
    void start();

    /**
	 * Issue a measurement command for the current state.
	 *
	 * @return		OK if the measurement command was successful.
	 */
    int measure();

    /**
	 * Collect the result of the most recent measurement.
	 */
    int collect();
};
