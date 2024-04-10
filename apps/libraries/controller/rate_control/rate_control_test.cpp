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
#include <rate_control/rate_control.hpp>

using namespace matrix;

TEST(RateControlTest, AllZeroCase) {
    RateControl rate_control;
    Vector3f    torque = rate_control.update(Vector3f(), Vector3f(), Vector3f(), 0.f, false);
    EXPECT_EQ(torque, Vector3f());
}
