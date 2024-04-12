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

class EkfMeasurementSamplingTest : public ::testing::Test {
public:
    EkfMeasurementSamplingTest() :
        ::testing::Test(),
        _ekf{std::make_shared<Ekf>()} {};

    std::shared_ptr<Ekf> _ekf;

    void SetUp() override {
        _ekf->init(0);
    }
};

TEST_F(EkfMeasurementSamplingTest, baroDownSampling) {
    // TODO: implement
}
