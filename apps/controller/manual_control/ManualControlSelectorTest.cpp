/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ManualControlSelector.hpp"
#include <gtest/gtest.h>
#include <drivers/drv_hrt.h>

using namespace time_literals;

static constexpr uint64_t some_time = 12345678;

TEST(ManualControlSelector, RcInputContinuous) {
    ManualControlSelector selector;
    selector.setRcInMode(0);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    // Now provide input with the correct source.
    manual_control_setpoint_s input{};
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;

    for (int i = 0; i < 5; i++) {
        selector.updateWithNewInputSample(timestamp, input, 1);
        EXPECT_TRUE(selector.setpoint().valid);
        EXPECT_EQ(selector.setpoint().timestamp_sample, timestamp);
        EXPECT_EQ(selector.instance(), 1);
        EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
        timestamp += 100_ms;
        input.timestamp_sample = timestamp;
    }
}

TEST(ManualControlSelector, RcInputOnly) {
    ManualControlSelector selector;
    selector.setRcInMode(0);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    manual_control_setpoint_s input{};
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_0;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_FALSE(selector.setpoint().valid);

    timestamp += 100_ms;

    // Now provide input with the correct source.
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
    EXPECT_EQ(selector.instance(), 1);
}

TEST(ManualControlSelector, MavlinkInputOnly) {
    ManualControlSelector selector;
    selector.setRcInMode(1);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    manual_control_setpoint_s input{};
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_FALSE(selector.setpoint().valid);

    timestamp += 100_ms;

    // Now provide input with the correct source.
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_3;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_MAVLINK_3);
    EXPECT_EQ(selector.instance(), 1);

    timestamp += 100_ms;

    // But only the first MAVLink source wins, others are too late.
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_4;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_MAVLINK_3);
    EXPECT_EQ(selector.instance(), 1);
}

TEST(ManualControlSelector, AutoInput) {
    ManualControlSelector selector;
    selector.setRcInMode(2);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    manual_control_setpoint_s input{};
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
    EXPECT_EQ(selector.instance(), 0);

    timestamp += 100_ms;

    // Now provide input from MAVLink as well which should get ignored.
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_0;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
    EXPECT_EQ(selector.instance(), 0);

    timestamp += 500_ms;

    // Now we'll let RC time out, so it should switch to MAVLINK.
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_0;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_MAVLINK_0);
    EXPECT_EQ(selector.instance(), 1);
}

TEST(ManualControlSelector, FirstInput) {
    ManualControlSelector selector;
    selector.setRcInMode(3);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    manual_control_setpoint_s input{};
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
    EXPECT_EQ(selector.instance(), 0);

    timestamp += 100_ms;

    // Now provide input from MAVLink as well which should get ignored.
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_0;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
    EXPECT_EQ(selector.instance(), 0);

    timestamp += 500_ms;

    // Now we'll let RC time out, but it should NOT switch to MAVLINK because RC was first
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_0;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_FALSE(selector.setpoint().valid);
    EXPECT_FALSE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_MAVLINK_0);
    EXPECT_EQ(selector.instance(), -1);

    timestamp += 100_ms;

    // Provide input from RC again and it should get accepted because it was the first.
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
    EXPECT_EQ(selector.instance(), 0);
}

TEST(ManualControlSelector, DisabledInput) {
    ManualControlSelector selector;
    selector.setRcInMode(4);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    manual_control_setpoint_s input{};
    // Reject MAVLink stick input
    input.data_source      = manual_control_setpoint_s::SOURCE_MAVLINK_0;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_FALSE(selector.setpoint().valid);
    EXPECT_EQ(selector.instance(), -1);

    timestamp += 100_ms;

    // Reject RC stick input
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 1);

    EXPECT_FALSE(selector.setpoint().valid);
    EXPECT_EQ(selector.instance(), -1);
}

TEST(ManualControlSelector, RcTimeout) {
    ManualControlSelector selector;
    selector.setRcInMode(0);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    manual_control_setpoint_s input{};
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp;
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_TRUE(selector.setpoint().valid);
    EXPECT_TRUE(selector.setpoint().data_source == manual_control_setpoint_s::SOURCE_RC);
    EXPECT_EQ(selector.instance(), 0);

    timestamp += 600_ms;

    // Update, to make sure it notices the timeout
    selector.updateValidityOfChosenInput(timestamp);

    EXPECT_FALSE(selector.setpoint().valid);
    EXPECT_EQ(selector.instance(), -1);
}

TEST(ManualControlSelector, RcOutdated) {
    ManualControlSelector selector;
    selector.setRcInMode(0);
    selector.setTimeout(500_ms);

    uint64_t timestamp = some_time;

    manual_control_setpoint_s input{};
    input.data_source      = manual_control_setpoint_s::SOURCE_RC;
    input.timestamp_sample = timestamp - 600_ms; // First sample is already outdated
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_FALSE(selector.setpoint().valid);
    EXPECT_EQ(selector.instance(), -1);

    // If we update again it should still not get accepted
    selector.updateWithNewInputSample(timestamp, input, 0);

    EXPECT_FALSE(selector.setpoint().valid);
    EXPECT_EQ(selector.instance(), -1);
}
