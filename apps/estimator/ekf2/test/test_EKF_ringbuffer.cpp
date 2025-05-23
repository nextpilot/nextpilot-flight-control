/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include <math.h>
#include "EKF/ekf.h"

struct sample {
    uint64_t time_us;
    float    data[3];
};

class EkfRingBufferTest : public ::testing::Test {
public:
    sample              _x, _y, _z;
    RingBuffer<sample> *_buffer{nullptr};

    void SetUp() override {
        _buffer    = new RingBuffer<sample>(3);
        _x.time_us = 1000000;
        _x.data[0] = _x.data[1] = _x.data[2] = 1.5f;

        _y.time_us = 2000000;
        _y.data[0] = _y.data[1] = _y.data[2] = 3.0f;

        _z.time_us = 3000000;
        _z.data[0] = _z.data[1] = _z.data[2] = 4.0f;
    }

    void TearDown() override {
        delete _buffer;
    }
};

TEST_F(EkfRingBufferTest, goodInitialisation) {
    // WHEN: buffer was allocated
    // THEN: allocation should have succeed
    ASSERT_EQ(true, _buffer->allocate(3));
}

TEST_F(EkfRingBufferTest, badInitialisation) {
    // WHEN: buffer allocation input is bad
    // THEN: allocation should fail

    // TODO: Change buffer implementation to pass this test
    // ASSERT_EQ(false, _buffer->allocate(-1));
    // ASSERT_EQ(false, _buffer->allocate(0));
}

TEST_F(EkfRingBufferTest, orderOfSamples) {
    ASSERT_EQ(true, _buffer->allocate(3));
    // GIVEN: allocated buffer
    // WHEN: adding multiple samples
    // THEN: they should be added in order
    _buffer->push(_x);

    EXPECT_EQ(_x.time_us, _buffer->get_newest().time_us);
    EXPECT_EQ(_x.time_us, _buffer->get_oldest().time_us);

    _buffer->push(_z);
    _buffer->push(_y);

    EXPECT_EQ(_x.time_us, _buffer->get_oldest().time_us);
    EXPECT_EQ(_y.time_us, _buffer->get_newest().time_us);
}

TEST_F(EkfRingBufferTest, popSample) {
    ASSERT_EQ(true, _buffer->allocate(3));
    _buffer->push(_x);
    _buffer->push(_y);
    _buffer->push(_z);

    // GIVEN: allocated and filled buffer

    sample pop = {};
    // WHEN: we want to retrieve a sample that is older than any in the buffer
    // THEN: we should not get any sample
    EXPECT_EQ(false, _buffer->pop_first_older_than(0, &pop));

    // WHEN: when calling "pop_first_older_than"
    // THEN: we should get the first sample from the head that is older
    EXPECT_EQ(true, _buffer->pop_first_older_than(_x.time_us + 1, &pop));
    EXPECT_EQ(_x.time_us, pop.time_us);
    EXPECT_EQ(true, _buffer->pop_first_older_than(_y.time_us + 10, &pop));
    EXPECT_EQ(_y.time_us, pop.time_us);
    EXPECT_EQ(true, _buffer->pop_first_older_than(_z.time_us + 100, &pop));
    EXPECT_EQ(_z.time_us, pop.time_us);
    // TODO: When changing the order of popping sample it does not behave as expected, fix this
}

TEST_F(EkfRingBufferTest, askingForTooNewSample) {
    ASSERT_EQ(true, _buffer->allocate(3));
    _buffer->push(_x);
    _buffer->push(_y);
    _buffer->push(_z);

    sample pop = {};
    // WHEN: all buffered samples are older by 0.1s than your query timestamp
    // THEN: should get no sample returned
    EXPECT_EQ(true, _buffer->pop_first_older_than(_z.time_us + 99000, &pop));
    EXPECT_EQ(false, _buffer->pop_first_older_than(_y.time_us + 100000, &pop));
}

TEST_F(EkfRingBufferTest, reallocateBuffer) {
    ASSERT_EQ(true, _buffer->allocate(5));
    _buffer->push(_x);
    _buffer->push(_y);
    _buffer->push(_y);
    _buffer->push(_y);
    _buffer->push(_z);

    // GIVEN: allocated and filled buffer
    // WHEN: do another allocate call
    _buffer->allocate(3);
    // THEN: its length should update
    EXPECT_EQ(3, _buffer->get_length());
}
