/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "MovingDiff.hpp"
#include <gtest/gtest.h>

TEST(MovingDiffTest, RcInputContinuous) {
    MovingDiff _diff;
    EXPECT_FLOAT_EQ(_diff.update(0.0f, 0.0f), 0.f); // 0,0,0
    EXPECT_FLOAT_EQ(_diff.update(1.0f, 1.0f), 0.f); // 1*,0,0
    EXPECT_FLOAT_EQ(_diff.update(0.0f, 1.0f), 0.f); // 1,-1*,0
    EXPECT_FLOAT_EQ(_diff.update(0.0f, 1.0f), 0.f); // 0,-1,0*
    EXPECT_FLOAT_EQ(_diff.update(0.0f, 1.0f), 0.f); // 0*,-1,0
    EXPECT_FLOAT_EQ(_diff.update(1.0f, 1.0f), 0.f); // 0,1*,0
    EXPECT_FLOAT_EQ(_diff.update(0.0f, 1.0f), 0.f); // 0,1,-1*
    EXPECT_FLOAT_EQ(_diff.update(2.0f, 1.0f), 1.f); // 2*,1,-1
    EXPECT_FLOAT_EQ(_diff.update(4.0f, 1.0f), 2.f); // 2,2*,-1
}
