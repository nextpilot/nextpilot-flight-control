#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

File: derivation_yaw_estimator.py
Description:
"""

import symforce.symbolic as sf
from derivation_utils import *


class State:
    vx = 0
    vy = 1
    yaw = 2
    n_states = 3


class VState(sf.Matrix):
    SHAPE = (State.n_states, 1)


class MState(sf.Matrix):
    SHAPE = (State.n_states, State.n_states)


def yaw_est_predict_covariance(state: VState, P: MState, d_vel: sf.V2, d_vel_var: sf.Scalar, d_ang_var: sf.Scalar):
    d_ang = sf.Symbol("d_ang")  # does not appear in the jacobians

    # derive the body to nav direction transformation matrix
    Tbn = sf.Matrix(
        [[sf.cos(state[State.yaw]), -sf.sin(state[State.yaw])], [sf.sin(state[State.yaw]), sf.cos(state[State.yaw])]]
    )

    # attitude update equation
    yaw_new = state[State.yaw] + d_ang

    # velocity update equations
    v_new = sf.V2(state[State.vx], state[State.vy]) + Tbn * d_vel

    # Define vector of process equations
    state_new = VState.block_matrix([[v_new], [sf.V1(yaw_new)]])

    # Calculate state transition matrix
    F = state_new.jacobian(state)

    # Derive the covariance prediction equations
    # Error growth in the inertial solution is assumed to be driven by 'noise' in the delta angles and
    # velocities, after bias effects have been removed.

    # derive the control(disturbance) influence matrix from IMU noise to state noise
    G = state_new.jacobian(sf.V3.block_matrix([[d_vel], [sf.V1(d_ang)]]))

    # derive the state error matrix
    var_u = sf.Matrix.diag([d_vel_var, d_vel_var, d_ang_var])

    Q = G * var_u * G.T

    P_new = F * P * F.T + Q

    # Generate the equations for the upper triangular matrix and the diagonal only
    # Since the matrix is symmetric, the lower triangle does not need to be derived
    # and can simply be copied in the implementation
    for index in range(State.n_states):
        for j in range(State.n_states):
            if index > j:
                P_new[index, j] = 0

    return P_new


def yaw_est_compute_measurement_update(P: MState, vel_obs_var: sf.Scalar, epsilon: sf.Scalar):
    H = sf.Matrix([[1, 0, 0], [0, 1, 0]])

    R = sf.Matrix([[vel_obs_var, 0], [0, vel_obs_var]])

    S = H * P * H.T + R
    S_det = S[0, 0] * S[1, 1] - S[1, 0] * S[0, 1]
    S_det_inv = add_epsilon_sign(1 / S_det, S_det, epsilon)

    # Compute inverse using simple formula for 2x2 matrix and using protected division
    S_inv = sf.M22([[S[1, 1], -S[0, 1]], [-S[1, 0], S[0, 0]]]) * S_det_inv
    K = (P * H.T) * S_inv
    P_new = P - K * H * P

    # Generate the equations for the upper triangular matrix and the diagonal only
    # Since the matrix is symmetric, the lower triangle does not need to be derived
    # and can simply be copied in the implementation
    for index in range(State.n_states):
        for j in range(State.n_states):
            if index > j:
                P_new[index, j] = 0

    return (S_inv, S_det_inv, K, P_new)


print("Derive yaw estimator equations...")
generate_px4_function(yaw_est_predict_covariance, output_names=["P_new"])
generate_px4_function(yaw_est_compute_measurement_update, output_names=["S_inv", "S_det_inv", "K", "P_new"])
