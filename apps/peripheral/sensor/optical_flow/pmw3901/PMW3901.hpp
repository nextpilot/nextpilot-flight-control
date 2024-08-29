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
 * @file PMW3901.hpp
 * @author Daniele Pettenuzzo
 *
 * Driver for the pmw3901 optical flow sensor connected via SPI.
 */

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#include <drivers/device/spi.h>
#include <conversion/rotation.h>
#include <lib/perf/perf_counter.h>
#include <drivers/drv_hrt.h>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/sensor_optical_flow.h>
#include <px4_platform_common/i2c_spi_buses.h>

/* Configuration Constants */

#define PMW3901_SPI_BUS_SPEED (2000000L) // 2MHz

#define DIR_WRITE(a)          ((a) | (1 << 7))
#define DIR_READ(a)           ((a) & 0x7f)

/* PMW3901 Registers addresses */
#define PMW3901_US              1000  /*   1 ms */
#define PMW3901_SAMPLE_INTERVAL 10000 /*  10 ms */

class PMW3901 : public device::SPI, public I2CSPIDriver<PMW3901> {
public:
    PMW3901(const I2CSPIDriverConfig &config);

    virtual ~PMW3901();

    static void print_usage();

    virtual int init();

    void print_status();

    void RunImpl();

protected:
    virtual int probe();

private:
    const uint64_t _collect_time{15000}; // usecs, ensures flow data is published every second iteration of Run() (100Hz -> 50Hz)

    uORB::PublicationMulti<sensor_optical_flow_s> _sensor_optical_flow_pub{ORB_ID(sensor_optical_flow)};

    perf_counter_t _sample_perf;
    perf_counter_t _comms_errors;

    uint64_t _previous_collect_timestamp{0};

    enum Rotation _yaw_rotation {
    };

    int      _flow_sum_x{0};
    int      _flow_sum_y{0};
    uint64_t _flow_dt_sum_usec{0};
    uint16_t _flow_quality_sum{0};
    uint8_t  _flow_sample_counter{0};

    /**
	* Initialise the automatic measurement state machine and start it.
	*
	* @note This function is called at open and error time.  It might make sense
	*       to make it more aggressive about resetting the bus in case of errors.
	*/
    void start();

    /**
	* Stop the automatic measurement state machine.
	*/
    void stop();


    int readRegister(unsigned reg, uint8_t *data, unsigned count);
    int writeRegister(unsigned reg, uint8_t data);

    int sensorInit();
    int readMotionCount(int16_t &deltaX, int16_t &deltaY, uint8_t &qual);
};
