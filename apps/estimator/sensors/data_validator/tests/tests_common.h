/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ECL_TESTS_COMMON_H
#define ECL_TESTS_COMMON_H

#include <validation/data_validator.h>

/**
 * Insert a series of samples around a mean value
 * @param validator The validator under test
 * @param mean The mean value
 * @param count The number of samples to insert in the validator
 * @param rms_err (out) calculated rms error of the inserted samples
 * @param timestamp_io (in/out) in: start timestamp, out: last timestamp
 */
void insert_values_around_mean(DataValidator *validator, const float mean, uint32_t count, float *rms_err,
                               uint64_t *timestamp_io);

/**
 * Print out the state of a DataValidator
 * @param validator
 */
void dump_validator_state(DataValidator *validator);

/**
 * Insert a time series of samples into the validator
 * @param validator
 * @param incr_value The amount to increment the value by on each iteration
 * @param value_io (in/out) in: initial value, out: final value
 * @param timestamp_io (in/out) in: initial timestamp, out: final timestamp
 */
void fill_validator_with_samples(DataValidator *validator,
                                 const float    incr_value,
                                 float         *value_io,
                                 uint64_t      *timestamp_io);

#endif //ECL_TESTS_COMMON_H
