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

File: derivation.py
Description:
    Derivation of a wind and airspeed scale (EKF) estimator using SymForce
"""

from symforce import symbolic as sm
from symforce import geo
from symforce import typing as T
from derivation_utils import *


def fuse_airspeed(
    v_local: geo.V3, state: geo.V3, P: geo.M33, airspeed: T.Scalar, R: T.Scalar, epsilon: T.Scalar
) -> (geo.V3, geo.V3, T.Scalar, T.Scalar):

    vel_rel = geo.V3(v_local[0] - state[0], v_local[1] - state[1], v_local[2])
    airspeed_pred = vel_rel.norm(epsilon=epsilon) * state[2]

    innov = airspeed - airspeed_pred

    H = geo.V1(airspeed_pred).jacobian(state)
    innov_var = (H * P * H.T + R)[0, 0]

    K = P * H.T / sm.Max(innov_var, epsilon)

    return (geo.V3(H), K, innov_var, innov)


def fuse_beta(
    v_local: geo.V3, state: geo.V3, P: geo.M33, q_att: geo.V4, R: T.Scalar, epsilon: T.Scalar
) -> (geo.V3, geo.V3, T.Scalar, T.Scalar):

    vel_rel = geo.V3(v_local[0] - state[0], v_local[1] - state[1], v_local[2])
    relative_wind_body = quat_to_rot(q_att).T * vel_rel

    # Small angle approximation of side slip model
    # Protect division by zero using epsilon
    beta_pred = add_epsilon_sign(relative_wind_body[1] / relative_wind_body[0], relative_wind_body[0], epsilon)

    innov = 0.0 - beta_pred

    H = geo.V1(beta_pred).jacobian(state)
    innov_var = (H * P * H.T + R)[0, 0]

    K = P * H.T / sm.Max(innov_var, epsilon)

    return (geo.V3(H), K, innov_var, innov)


def init_wind_using_airspeed(
    v_local: geo.V3,
    heading: T.Scalar,
    airspeed: T.Scalar,
    v_var: T.Scalar,
    heading_var: T.Scalar,
    sideslip_var: T.Scalar,
    airspeed_var: T.Scalar,
) -> (geo.V2, geo.M22):

    # Initialise wind states assuming zero side slip and horizontal flight
    wind = geo.V2(v_local[0] - airspeed * sm.cos(heading), v_local[1] - airspeed * sm.sin(heading))
    # Zero sideslip, propagate the sideslip variance using partial derivatives w.r.t heading
    J = wind.jacobian([v_local[0], v_local[1], heading, heading, airspeed])

    R = geo.M55()
    R[0, 0] = v_var
    R[1, 1] = v_var
    R[2, 2] = heading_var
    R[3, 3] = sideslip_var
    R[4, 4] = airspeed_var

    P = J * R * J.T

    return (wind, P)


generate_px4_function(fuse_airspeed, output_names=["H", "K", "innov_var", "innov"])
generate_px4_function(fuse_beta, output_names=["H", "K", "innov_var", "innov"])
generate_px4_function(init_wind_using_airspeed, output_names=["wind", "P"])

generate_python_function(fuse_airspeed, output_names=["H", "K", "innov_var", "innov"])
