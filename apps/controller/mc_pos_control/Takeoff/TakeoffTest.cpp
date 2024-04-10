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
#include <Takeoff.hpp>
#include <hrtimer.h>
#include <geo/geo.h>

TEST(TakeoffTest, Initialization) {
    TakeoffHandling takeoff;
    EXPECT_EQ(takeoff.getTakeoffState(), TakeoffState::disarmed);
}

TEST(TakeoffTest, RegularTakeoffRamp) {
    TakeoffHandling takeoff;
    takeoff.setSpoolupTime(1.f);
    takeoff.setTakeoffRampTime(2.0);
    takeoff.generateInitialRampValue(CONSTANTS_ONE_G / 0.5f);

    // disarmed, landed, don't want takeoff
    takeoff.updateTakeoffState(false, true, false, 1.f, false, 0);
    EXPECT_EQ(takeoff.getTakeoffState(), TakeoffState::disarmed);

    // armed, not landed anymore, don't want takeoff
    takeoff.updateTakeoffState(true, false, false, 1.f, false, 500_ms);
    EXPECT_EQ(takeoff.getTakeoffState(), TakeoffState::spoolup);

    // armed, not landed, don't want takeoff yet, spoolup time passed
    takeoff.updateTakeoffState(true, false, false, 1.f, false, 2_s);
    EXPECT_EQ(takeoff.getTakeoffState(), TakeoffState::ready_for_takeoff);

    // armed, not landed, want takeoff
    takeoff.updateTakeoffState(true, false, true, 1.f, false, 3_s);
    EXPECT_EQ(takeoff.getTakeoffState(), TakeoffState::rampup);

    // armed, not landed, want takeoff, ramping up
    takeoff.updateTakeoffState(true, false, true, 1.f, false, 4_s);
    EXPECT_FLOAT_EQ(takeoff.updateRamp(.5f, 1.5f), 0.f);
    EXPECT_FLOAT_EQ(takeoff.updateRamp(.5f, 1.5f), .5f);
    EXPECT_FLOAT_EQ(takeoff.updateRamp(.5f, 1.5f), 1.f);
    EXPECT_FLOAT_EQ(takeoff.updateRamp(.5f, 1.5f), 1.5f);
    EXPECT_FLOAT_EQ(takeoff.updateRamp(.5f, 1.5f), 1.5f);

    // armed, not landed, want takeoff, rampup time passed
    takeoff.updateTakeoffState(true, false, true, 1.f, false, 6500_ms);
    EXPECT_EQ(takeoff.getTakeoffState(), TakeoffState::flight);
}
