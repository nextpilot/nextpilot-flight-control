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
 * @file pid.c
 *
 * Implementation of generic PID controller.
 *
 * @author Laurens Mackay <mackayl@student.ethz.ch>
 * @author Tobias Naegeli <naegelit@student.ethz.ch>
 * @author Martin Rutschmann <rutmarti@student.ethz.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 * @author Julian Oes <joes@student.ethz.ch>
 */

#include "pid.h"
#include <math.h>
#include <px4_platform_common/defines.h>

#define SIGMA 0.000001f

__EXPORT void pid_init(PID_t *pid, pid_mode_t mode, float dt_min)
{
	pid->mode = mode;
	pid->dt_min = dt_min;
	pid->kp = 0.0f;
	pid->ki = 0.0f;
	pid->kd = 0.0f;
	pid->integral = 0.0f;
	pid->integral_limit = 0.0f;
	pid->output_limit = 0.0f;
	pid->error_previous = 0.0f;
	pid->last_output = 0.0f;
}

__EXPORT int pid_set_parameters(PID_t *pid, float kp, float ki, float kd, float integral_limit, float output_limit)
{
	int ret = 0;

	if (PX4_ISFINITE(kp)) {
		pid->kp = kp;

	} else {
		ret = 1;
	}

	if (PX4_ISFINITE(ki)) {
		pid->ki = ki;

	} else {
		ret = 1;
	}

	if (PX4_ISFINITE(kd)) {
		pid->kd = kd;

	} else {
		ret = 1;
	}

	if (PX4_ISFINITE(integral_limit)) {
		pid->integral_limit = integral_limit;

	}  else {
		ret = 1;
	}

	if (PX4_ISFINITE(output_limit)) {
		pid->output_limit = output_limit;

	}  else {
		ret = 1;
	}

	return ret;
}

__EXPORT float pid_calculate(PID_t *pid, float sp, float val, float val_dot, float dt)
{
	if (!PX4_ISFINITE(sp) || !PX4_ISFINITE(val) || !PX4_ISFINITE(val_dot) || !PX4_ISFINITE(dt)) {
		return pid->last_output;
	}

	float i, d;

	/* current error value */
	float error = sp - val;

	/* current error derivative */
	if (pid->mode == PID_MODE_DERIVATIV_CALC) {
		d = (error - pid->error_previous) / fmaxf(dt, pid->dt_min);
		pid->error_previous = error;

	} else if (pid->mode == PID_MODE_DERIVATIV_CALC_NO_SP) {
		d = (-val - pid->error_previous) / fmaxf(dt, pid->dt_min);
		pid->error_previous = -val;

	} else if (pid->mode == PID_MODE_DERIVATIV_SET) {
		d = -val_dot;

	} else {
		d = 0.0f;
	}

	if (!PX4_ISFINITE(d)) {
		d = 0.0f;
	}

	/* calculate PD output */
	float output = (error * pid->kp) + (d * pid->kd);

	if (pid->ki > SIGMA) {
		// Calculate the error integral and check for saturation
		i = pid->integral + (error * dt);

		/* check for saturation */
		if (PX4_ISFINITE(i)) {
			if ((pid->output_limit < SIGMA || (fabsf(output + (i * pid->ki)) <= pid->output_limit)) &&
			    fabsf(i) <= pid->integral_limit) {
				/* not saturated, use new integral value */
				pid->integral = i;
			}
		}

		/* add I component to output */
		output += pid->integral * pid->ki;
	}

	/* limit output */
	if (PX4_ISFINITE(output)) {
		if (pid->output_limit > SIGMA) {
			if (output > pid->output_limit) {
				output = pid->output_limit;

			} else if (output < -pid->output_limit) {
				output = -pid->output_limit;
			}
		}

		pid->last_output = output;
	}

	return pid->last_output;
}


__EXPORT void pid_reset_integral(PID_t *pid)
{
	pid->integral = 0.0f;
}
