/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <stdio.h>
#include "tests_common.h"

void insert_values_around_mean(DataValidator *validator, const float mean, uint32_t count, float *rms_err,
                               uint64_t *timestamp_io) {
    uint64_t       timestamp       = *timestamp_io;
    uint64_t       timestamp_incr  = 5;
    const uint32_t error_count     = 0;
    const uint8_t  priority        = 50;
    const float    swing           = 1E-2f;
    double         sum_dev_squares = 0.0f;

    //insert a series of values that swing around the mean
    for (uint32_t i = 0; i < count; i++) {
        float iter_swing  = (0 == (i % 2)) ? swing : -swing;
        float iter_val    = mean + iter_swing;
        float iter_dev    = iter_val - mean;
        sum_dev_squares  += (iter_dev * iter_dev);
        timestamp        += timestamp_incr;
        validator->put(timestamp, iter_val, error_count, priority);
    }

    double rms = sqrt(sum_dev_squares / (double)count);
    //note: this should be approximately equal to "swing"
    *rms_err      = (float)rms;
    *timestamp_io = timestamp;
}

void dump_validator_state(DataValidator *validator) {
    uint32_t state = validator->state();
    printf("state: 0x%x no_data: %d stale: %d timeout:%d\n",
           validator->state(),
           DataValidator::ERROR_FLAG_NO_DATA & state,
           DataValidator::ERROR_FLAG_STALE_DATA & state,
           DataValidator::ERROR_FLAG_TIMEOUT & state);
    validator->print();
    printf("\n");
}

void fill_validator_with_samples(DataValidator *validator,
                                 const float    incr_value,
                                 float         *value_io,
                                 uint64_t      *timestamp_io) {
    uint64_t       timestamp      = *timestamp_io;
    const uint64_t timestamp_incr = 5;    //usec
    const uint32_t timeout_usec   = 2000; //derived from class-private value

    float          val               = *value_io;
    const uint32_t error_count       = 0;
    const uint8_t  priority          = 50;  //"medium" priority
    const int      equal_value_count = 100; //default is private VALUE_EQUAL_COUNT_DEFAULT

    validator->set_equal_value_threshold(equal_value_count);
    validator->set_timeout(timeout_usec);

    //put a bunch of values that are all different
    for (int i = 0; i < equal_value_count; i++, val += incr_value) {
        timestamp += timestamp_incr;
        validator->put(timestamp, val, error_count, priority);
    }

    *timestamp_io = timestamp;
    *value_io     = val;
}
