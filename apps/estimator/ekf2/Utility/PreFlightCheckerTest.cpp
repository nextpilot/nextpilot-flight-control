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
 * Test code for PreFlightChecker class
 * Run this test only using make tests TESTFILTER=PreFlightChecker
 */

#include <gtest/gtest.h>

#include "PreFlightChecker.hpp"

class PreFlightCheckerTest : public ::testing::Test {
};

TEST_F(PreFlightCheckerTest, testInnovFailed) {
    const float test_limit         = 1.0;              ///< is the limit for innovation_lpf
    const float spike_limit        = 2.f * test_limit; ///< is the limit for innovation_lpf
    const float innovations[9]     = {0.0, 1.5, 2.5, -1.5, -2.5, 1.5, -1.5, -2.5, -2.5};
    const float innovations_lpf[9] = {0.0, 0.9, 0.9, -0.9, -0.9, 1.1, -1.1, -1.1, 1.1};
    const bool  expected_result[9] = {false, false, true, false, true, true, true, true, true};

    for (int i = 0; i < 9; i++) {
        EXPECT_EQ(PreFlightChecker::checkInnovFailed(innovations_lpf[i], innovations[i], test_limit, spike_limit),
                  expected_result[i]);
    }

    // Smaller test limit, all the checks should fail except the first
    EXPECT_FALSE(PreFlightChecker::checkInnovFailed(innovations_lpf[0], innovations[0], 0.0, 0.0));

    for (int i = 1; i < 9; i++) {
        EXPECT_TRUE(PreFlightChecker::checkInnovFailed(innovations_lpf[i], innovations[i], 0.0, 0.0));
    }

    // Larger test limit, none of the checks should fail
    for (int i = 0; i < 9; i++) {
        EXPECT_FALSE(PreFlightChecker::checkInnovFailed(innovations_lpf[i], innovations[i], 2.0, 4.0));
    }
}

TEST_F(PreFlightCheckerTest, testInnov2dFailed) {
    const float test_limit         = 1.0;
    const float spike_limit        = 2.0;
    Vector2f    innovations[4]     = {{0.0, 0.0}, {0.0, 0.0}, {0.0, -2.5}, {1.5, -1.5}};
    Vector2f    innovations_lpf[4] = {{0.0, 0.0}, {1.1, 0.0}, {0.5, 0.5}, {1.0, -1.0}};
    const bool  expected_result[4] = {false, true, true, true};

    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(PreFlightChecker::checkInnov2DFailed(innovations_lpf[i], innovations[i], test_limit, spike_limit),
                  expected_result[i]);
    }

    // Smaller test limit, all the checks should fail except the first
    EXPECT_FALSE(PreFlightChecker::checkInnov2DFailed(innovations[0], innovations_lpf[0], 0.0, 0.0));

    for (int i = 1; i < 4; i++) {
        EXPECT_TRUE(PreFlightChecker::checkInnov2DFailed(innovations_lpf[i], innovations[i], 0.0, 0.0));
    }

    // Larger test limit, none of the checks should fail
    for (int i = 0; i < 4; i++) {
        EXPECT_FALSE(PreFlightChecker::checkInnov2DFailed(innovations_lpf[i], innovations[i], 1.42, 2.84));
    }
}
