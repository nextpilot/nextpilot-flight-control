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
 * @file test_EKF_utils.cpp
 *
 * @brief Unit tests for the miscellaneous EKF utilities
 */

#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <mathlib/mathlib.h>

#include "EKF/utils.hpp"

TEST(eclPowfTest, compareToStandardImplementation) {
    std::vector<int>   exponents = {-3, -2, -1, -0, 0, 1, 2, 3};
    std::vector<float> bases     = {-INFINITY, -11.1f, -0.5f, -0.f, 0.f, 0.5f, 11.1f, INFINITY};

    for (auto const exponent : exponents) {
        for (auto const basis : bases) {
            EXPECT_EQ(ecl::powf(basis, exponent),
                      std::pow(basis, static_cast<float>(exponent)));
        }
    }
}
