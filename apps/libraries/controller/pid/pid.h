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
 * @file pid.h
 *
 * Definition of generic PID controller.
 *
 * @author Laurens Mackay <mackayl@student.ethz.ch>
 * @author Tobias Naegeli <naegelit@student.ethz.ch>
 * @author Martin Rutschmann <rutmarti@student.ethz.ch>
 * @author Anton Babushkin <anton.babushkin@me.com>
 * @author Julian Oes <joes@student.ethz.ch>
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>

__BEGIN_DECLS

typedef enum PID_MODE {
	/* Use PID_MODE_DERIVATIV_NONE for a PI controller (vs PID) */
	PID_MODE_DERIVATIV_NONE = 0,
	/* PID_MODE_DERIVATIV_CALC calculates discrete derivative from previous error,
	 * val_dot in pid_calculate() will be ignored */
	PID_MODE_DERIVATIV_CALC,
	/* PID_MODE_DERIVATIV_CALC_NO_SP calculates discrete derivative from previous value,
	 * setpoint derivative will be ignored, val_dot in pid_calculate() will be ignored */
	PID_MODE_DERIVATIV_CALC_NO_SP,
	/* Use PID_MODE_DERIVATIV_SET if you have the derivative already (Gyros, Kalman) */
	PID_MODE_DERIVATIV_SET
} pid_mode_t;

typedef struct {
	pid_mode_t mode;
	float dt_min;
	float kp;
	float ki;
	float kd;
	float integral;
	float integral_limit;
	float output_limit;
	float error_previous;
	float last_output;
} PID_t;

__EXPORT void pid_init(PID_t *pid, pid_mode_t mode, float dt_min);
__EXPORT int pid_set_parameters(PID_t *pid, float kp, float ki, float kd, float integral_limit, float output_limit);
__EXPORT float pid_calculate(PID_t *pid, float sp, float val, float val_dot, float dt);
__EXPORT void pid_reset_integral(PID_t *pid);

__END_DECLS

#endif /* PID_H_ */
