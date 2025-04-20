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
 * @file second_order_reference_model_test.cpp
 *
 * @brief Unit tests for the second order reference model implementation
 */

#include <gtest/gtest.h>
#include <matrix/math.hpp>
#include <mathlib/math/filter/second_order_reference_model.hpp>

using math::SecondOrderReferenceModel;
using matrix::Vector3f;

TEST(SecondOrderReferenceModel, FloatDefaultConstructorInitializesStatesToZero)
{
	SecondOrderReferenceModel<float> sys_float;

	// default constructor leaves states initialized to zero
	EXPECT_FLOAT_EQ(sys_float.getState(), 0.0f);
	EXPECT_FLOAT_EQ(sys_float.getRate(), 0.0f);
	EXPECT_FLOAT_EQ(sys_float.getAccel(), 0.0f);
}

TEST(SecondOrderReferenceModel, FloatReset)
{
	SecondOrderReferenceModel<float> sys_float;

	// reset the system states
	float init_state = 3.0f;
	float init_rate = 1.1f;
	sys_float.reset(init_state, init_rate);
	EXPECT_FLOAT_EQ(sys_float.getState(), init_state);
	EXPECT_FLOAT_EQ(sys_float.getRate(), init_rate);
	EXPECT_FLOAT_EQ(sys_float.getAccel(), 0.0f);

	// reset the system without providing rate
	init_state = 5.5f;
	sys_float.reset(init_state);
	EXPECT_FLOAT_EQ(sys_float.getState(), init_state);
	EXPECT_FLOAT_EQ(sys_float.getRate(), 0.0f);
	EXPECT_FLOAT_EQ(sys_float.getAccel(), 0.0f);
}

TEST(SecondOrderReferenceModel, Vector3DefaultConstructorInitializesStatesToZero)
{
	SecondOrderReferenceModel<Vector3f> sys_vector3f;

	// default constructor leaves states initialized to zero
	Vector3f state = sys_vector3f.getState();
	EXPECT_FLOAT_EQ(state(0), 0.0f);
	EXPECT_FLOAT_EQ(state(1), 0.0f);
	EXPECT_FLOAT_EQ(state(2), 0.0f);
	Vector3f rate = sys_vector3f.getRate();
	EXPECT_FLOAT_EQ(rate(0), 0.0f);
	EXPECT_FLOAT_EQ(rate(1), 0.0f);
	EXPECT_FLOAT_EQ(rate(2), 0.0f);
	Vector3f accel = sys_vector3f.getAccel();
	EXPECT_FLOAT_EQ(accel(0), 0.0f);
	EXPECT_FLOAT_EQ(accel(1), 0.0f);
	EXPECT_FLOAT_EQ(accel(2), 0.0f);
}

TEST(SecondOrderReferenceModel, Vector3Reset)
{
	SecondOrderReferenceModel<Vector3f> sys_vector3f;

	// reset the system states
	Vector3f init_state(1.0f, 2.0f, 3.0f);
	Vector3f init_rate(0.1f, 0.2f, 0.3f);
	sys_vector3f.reset(init_state, init_rate);
	Vector3f state = sys_vector3f.getState();
	EXPECT_FLOAT_EQ(state(0), init_state(0));
	EXPECT_FLOAT_EQ(state(1), init_state(1));
	EXPECT_FLOAT_EQ(state(2), init_state(2));
	Vector3f rate = sys_vector3f.getRate();
	EXPECT_FLOAT_EQ(rate(0), init_rate(0));
	EXPECT_FLOAT_EQ(rate(1), init_rate(1));
	EXPECT_FLOAT_EQ(rate(2), init_rate(2));
	Vector3f accel = sys_vector3f.getAccel();
	EXPECT_FLOAT_EQ(accel(0), 0.0f);
	EXPECT_FLOAT_EQ(accel(1), 0.0f);
	EXPECT_FLOAT_EQ(accel(2), 0.0f);

	// reset the system without providing rate
	init_state = Vector3f(4.0f, 5.0f, 6.0f);
	sys_vector3f.reset(init_state);
	state = sys_vector3f.getState();
	EXPECT_FLOAT_EQ(state(0), init_state(0));
	EXPECT_FLOAT_EQ(state(1), init_state(1));
	EXPECT_FLOAT_EQ(state(2), init_state(2));
	rate = sys_vector3f.getRate();
	EXPECT_FLOAT_EQ(rate(0), 0.0f);
	EXPECT_FLOAT_EQ(rate(1), 0.0f);
	EXPECT_FLOAT_EQ(rate(2), 0.0f);
	accel = sys_vector3f.getAccel();
	EXPECT_FLOAT_EQ(accel(0), 0.0f);
	EXPECT_FLOAT_EQ(accel(1), 0.0f);
	EXPECT_FLOAT_EQ(accel(2), 0.0f);
}
