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
 * Test code for the Velocity Smoothing library
 * Build and run using: make && ./test_velocity_smoothing
 */

#include "VelocitySmoothing.hpp"
#include <cstdio>
#include <matrix/matrix/math.hpp>

int main(int argc, char *argv[])
{
	VelocitySmoothing trajectory[3];

	float a0[3] = {0.f, 0.f, 0.f};
	float v0[3] = {0.f, 0.f, 0.f};
	float x0[3] = {0.f, 0.f, 0.f};

	float j_max = 55.2f;
	float a_max = 6.f;
	float v_max = 6.f;

	for (int i = 0; i < 3; i++) {
		trajectory[i].setMaxJerk(j_max);
		trajectory[i].setMaxAccel(a_max);
		trajectory[i].setMaxVel(v_max);
		trajectory[i].setCurrentAcceleration(a0[i]);
		trajectory[i].setCurrentVelocity(v0[i]);
	}

	const float dt = 0.01f;

	float velocity_setpoint[3] = {1.f, 0.f, -1.f};

	for (int i = 0; i < 3; i++) {
		trajectory[i].updateDurations(velocity_setpoint[i]);
	}

	float t123 = trajectory[0].getTotalTime();
	int nb_steps = ceil(t123 / dt);
	printf("Nb steps = %d\n", nb_steps);

	for (int i = 0; i < nb_steps; i++) {
		for (int i = 0; i < 3; i++) {
			trajectory[i].updateTraj(dt);
		}

		for (int i = 0; i < 3; i++) {
			trajectory[i].updateDurations(velocity_setpoint[i]);
		}

		VelocitySmoothing::timeSynchronization(trajectory, 2);

		for (int i = 0; i < 1; i++) {
			printf("Traj[%d]\n", i);
			printf("jerk = %.3f\taccel = %.3f\tvel = %.3f\tpos = %.3f\n",
			       trajectory[i].getCurrentJerk(),
			       trajectory[i].getCurrentAcceleration(),
			       trajectory[i].getCurrentVelocity(),
			       trajectory[i].getCurrentPosition());
			printf("T1 = %.3f\tT2 = %.3f\tT3 = %.3f\n", trajectory[i].getT1(), trajectory[i].getT2(), trajectory[i].getT3());
			printf("\n");
		}
	}

	return 0;
}
