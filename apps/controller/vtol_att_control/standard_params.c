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
 * @file standard_params.c
 * Parameters for the standard VTOL controller.
 *
 * @author Simon Wilks	<simon@uaventure.com>
 * @author Roman Bapst	<roman@px4.io>
 */

/**
 * Use fixed-wing actuation in hover to accelerate forward
 *
 * This feature can be used to avoid the plane having to pitch nose down in order to move forward.
 * Prevents large, negative lift from pitching nose down into wind.
 * Fixed-wing forward actuators refers to puller/pusher (standard VTOL), or forward-tilt (tiltrotor VTOL).
 * Only active if demanded down pitch is below VT_PITCH_MIN.
 * Use VT_FWD_THRUST_SC to tune it.
 *
 * Only active (if enabled) in Altitude, Position and Auto modes, not in Stabilized.
 *
 * @value 0 Disabled
 * @value 1 Enabled (except LANDING)
 * @value 2 Enabled if distance to ground above MPC_LAND_ALT1
 * @value 3 Enabled if distance to ground above MPC_LAND_ALT2
 * @value 4 Enabled constantly
 * @value 5 Enabled if distance to ground above MPC_LAND_ALT1 (except LANDING)
 * @value 6 Enabled if distance to ground above MPC_LAND_ALT2 (except LANDING)
 *
 * @group VTOL Attitude Control
 */
PARAM_DEFINE_INT32(VT_FWD_THRUST_EN, 0);

/**
 * Fixed-wing actuation thrust scale for hover forward flight
 *
 * Scale applied to the demanded down-pitch to get the fixed-wing forward actuation in hover mode.
 * Enabled via VT_FWD_THRUST_EN.
 *
 * @min 0.0
 * @max 2.0
 * @increment 0.01
 * @decimal 2
 * @group VTOL Attitude Control
 */
PARAM_DEFINE_FLOAT(VT_FWD_THRUST_SC, 0.7f);

/**
 * Back transition MC motor ramp up time
 *
 * This sets the duration during which the MC motors ramp up to the commanded thrust during the back transition stage.
 *
 * @unit s
 * @min 0.0
 * @max 20.0
 * @increment 0.1
 * @decimal 1
 * @group VTOL Attitude Control
 */
PARAM_DEFINE_FLOAT(VT_B_TRANS_RAMP, 3.0f);

/**
 * Pusher throttle ramp up slew rate
 *
 * Defines the slew rate of the puller/pusher throttle during transitions.
 * Zero will deactivate the slew rate limiting and thus produce an instant throttle
 * rise to the transition throttle VT_F_TRANS_THR.
 *
 * @unit 1/s
 * @min 0
 * @increment 0.01
 * @decimal 2
 * @group VTOL Attitude Control
 */
PARAM_DEFINE_FLOAT(VT_PSHER_SLEW, 0.33f);
