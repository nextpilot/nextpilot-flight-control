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
 * @file i2c_frame.h
 * Definition of i2c frames.
 * There are different I2C readouts available: i2c_frame and i2c_integral_frame.
 * The ic2_frame can be requested via the register 0x0, the i2c_integral_frame via 0x16.
 *
 * @author Thomas Boehm <thomas.boehm@fortiss.org>
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#include <inttypes.h>

typedef struct i2c_frame {
    uint16_t frame_count;      /**< counts created I2C frames [#frames] */
    int16_t  pixel_flow_x_sum; /**< latest x flow measurement in pixels*10 [pixels] */
    int16_t  pixel_flow_y_sum; /**< latest y flow measurement in pixels*10 [pixels] */
    int16_t  flow_comp_m_x;    /**< x velocity*1000 [meters/sec] */
    int16_t  flow_comp_m_y;    /**< y velocity*1000 [meters/sec] */
    int16_t  qual;             /**< Optical flow quality / confidence [0: bad, 255: maximum quality] */
    int16_t  gyro_x_rate;      /**< latest gyro x rate [rad/sec] */
    int16_t  gyro_y_rate;      /**< latest gyro y rate [rad/sec] */
    int16_t  gyro_z_rate;      /**< latest gyro z rate [rad/sec] */
    uint8_t  gyro_range;       /**< gyro range [0 .. 7] equals [50 deg/sec .. 2000 deg/sec] */
    uint8_t  sonar_timestamp;  /**< time since last sonar update [milliseconds] */
    int16_t  ground_distance;  /**< Ground distance in meters*1000 [meters]. Positive value: distance known. Negative value: Unknown distance */
} i2c_frame;

#define I2C_FRAME_SIZE (sizeof(i2c_frame))

typedef struct i2c_integral_frame {
    uint16_t frame_count_since_last_readout; /**< number of flow measurements since last I2C readout [#frames] */
    int16_t  pixel_flow_x_integral;          /**< accumulated flow in radians*10000 around x axis since last I2C readout [rad*10000] */
    int16_t  pixel_flow_y_integral;          /**< accumulated flow in radians*10000 around y axis since last I2C readout [rad*10000] */
    int16_t  gyro_x_rate_integral;           /**< accumulated gyro x rates in radians*10000 since last I2C readout [rad*10000] */
    int16_t  gyro_y_rate_integral;           /**< accumulated gyro y rates in radians*10000 since last I2C readout [rad*10000] */
    int16_t  gyro_z_rate_integral;           /**< accumulated gyro z rates in radians*10000 since last I2C readout [rad*10000] */
    uint32_t integration_timespan;           /**< accumulation timespan in microseconds since last I2C readout [microseconds] */
    uint32_t sonar_timestamp;                /**< time since last sonar update [microseconds] */
    uint16_t ground_distance;                /**< Ground distance in meters*1000 [meters*1000] */
    int16_t  gyro_temperature;               /**< Temperature * 100 in centi-degrees Celsius [degcelsius*100] */
    uint8_t  qual;                           /**< averaged quality of accumulated flow values [0:bad quality;255: max quality] */
} i2c_integral_frame;

#define I2C_INTEGRAL_FRAME_SIZE (sizeof(i2c_integral_frame))
