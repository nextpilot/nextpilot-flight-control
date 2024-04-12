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

File: derivation_terrain_estimator.py
Description:
"""

import symforce.symbolic as sf
from derivation_utils import *


def predict_opt_flow(terrain_vpos: sf.Scalar, q_att: sf.V4, v: sf.V3, pos_z: sf.Scalar, epsilon: sf.Scalar):
    R_to_earth = quat_to_rot(q_att)
    flow_pred = sf.V2()
    dist = -(pos_z - terrain_vpos)
    dist = add_epsilon_sign(dist, dist, epsilon)
    flow_pred[0] = -v[1] / dist * R_to_earth[2, 2]
    flow_pred[1] = v[0] / dist * R_to_earth[2, 2]
    return flow_pred


def terr_est_compute_flow_xy_innov_var_and_hx(
    terrain_vpos: sf.Scalar, P: sf.Scalar, q_att: sf.V4, v: sf.V3, pos_z: sf.Scalar, R: sf.Scalar, epsilon: sf.Scalar
):
    flow_pred = predict_opt_flow(terrain_vpos, q_att, v, pos_z, epsilon)
    Hx = sf.V1(flow_pred[0]).jacobian(terrain_vpos)
    Hy = sf.V1(flow_pred[1]).jacobian(terrain_vpos)

    innov_var = sf.V2()
    innov_var[0] = (Hx * P * Hx.T + R)[0, 0]
    innov_var[1] = (Hy * P * Hy.T + R)[0, 0]

    return (innov_var, Hx[0, 0])


def terr_est_compute_flow_y_innov_var_and_h(
    terrain_vpos: sf.Scalar, P: sf.Scalar, q_att: sf.V4, v: sf.V3, pos_z: sf.Scalar, R: sf.Scalar, epsilon: sf.Scalar
):
    flow_pred = predict_opt_flow(terrain_vpos, q_att, v, pos_z, epsilon)
    Hy = sf.V1(flow_pred[1]).jacobian(terrain_vpos)

    innov_var = (Hy * P * Hy.T + R)[0, 0]

    return (innov_var, Hy[0, 0])


print("Derive terrain estimator equations...")
generate_px4_function(terr_est_compute_flow_xy_innov_var_and_hx, output_names=["innov_var", "H"])
generate_px4_function(terr_est_compute_flow_y_innov_var_and_h, output_names=["innov_var", "H"])
