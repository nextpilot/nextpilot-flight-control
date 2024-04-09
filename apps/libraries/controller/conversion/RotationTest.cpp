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
 * @file RotationTest.cpp
 * Tests for rotations.
 */

#include <gtest/gtest.h>

#include "rotation.h"


TEST(Rotations, matrix_vs_3f)
{
	//iterate through all defined rotations
	for (size_t i = 0; i < (size_t)Rotation::ROTATION_MAX; i++) {

		// GIVEN: an initial vector and a rotation
		const matrix::Vector3f original = {1.f, 2.f, 3.f};
		const enum Rotation	rotation = static_cast<Rotation>(i);

		// WHEN: we transform the vector using the rotate_3f function and the rotation matrix
		matrix::Vector3f transformed_3f = original;
		rotate_3f(rotation, transformed_3f(0), transformed_3f(1), transformed_3f(2));

		matrix::Dcmf matrix = get_rot_matrix(rotation);
		matrix::Vector3f transformed_mat = matrix * original;

		// THEN: the results should be the same
		EXPECT_NEAR(transformed_mat(0), transformed_3f(0), 10e-6);
		EXPECT_NEAR(transformed_mat(1), transformed_3f(1), 10e-6);
		EXPECT_NEAR(transformed_mat(2), transformed_3f(2), 10e-6);
	}
}

TEST(Rotations, duplicates)
{
	// find all identical rotations to skip (needs to be kept in sync with mag calibration auto rotation)
	for (size_t i = 0; i < (size_t)Rotation::ROTATION_MAX; i++) {

		// GIVEN: an initial vector and a rotation
		const matrix::Vector3f original = {1.f, 2.f, 3.f};
		const enum Rotation rotation_1 = static_cast<Rotation>(i);

		// WHEN: we transform the vector using the rotate_3f function and the rotation matrix
		matrix::Vector3f transformed_1 = original;
		rotate_3f(rotation_1, transformed_1(0), transformed_1(1), transformed_1(2));

		for (size_t j = 0; j < (size_t)Rotation::ROTATION_MAX; j++) {

			const enum Rotation rotation_2 = static_cast<Rotation>(j);

			matrix::Vector3f transformed_2 = original;
			rotate_3f(rotation_2, transformed_2(0), transformed_2(1), transformed_2(2));

			if (i != j) {
				// ROTATION_PITCH_180_YAW_90 (26) = ROTATION_ROLL_180_YAW_270 (14)
				if (i == ROTATION_ROLL_180_YAW_270 && j == ROTATION_PITCH_180_YAW_90) { continue; }

				if (j == ROTATION_ROLL_180_YAW_270 && i == ROTATION_PITCH_180_YAW_90) { continue; }


				// ROTATION_ROLL_180_YAW_90 (10) = ROTATION_PITCH_180_YAW_270 (27)
				if (i == ROTATION_ROLL_180_YAW_90 && j == ROTATION_PITCH_180_YAW_270) { continue; }

				if (j == ROTATION_ROLL_180_YAW_90 && i == ROTATION_PITCH_180_YAW_270) { continue; }


				// otherwise all rotations should be different
				ASSERT_GT((transformed_1 - transformed_2).norm(), 0) << "Rotation " << i << " and " << j << " equal";
			}
		}
	}
}
