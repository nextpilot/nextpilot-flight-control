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

File: derivation_utils.py
Description:
    Common functions used for the derivation of most estimators
"""

import symforce.symbolic as sf

import re


# q: quaternion describing rotation from frame 1 to frame 2
# returns a rotation matrix derived form q which describes the same
# rotation
def quat_to_rot(q):
    q0 = q[0]
    q1 = q[1]
    q2 = q[2]
    q3 = q[3]

    Rot = sf.M33(
        [
            [q0**2 + q1**2 - q2**2 - q3**2, 2 * (q1 * q2 - q0 * q3), 2 * (q1 * q3 + q0 * q2)],
            [2 * (q1 * q2 + q0 * q3), q0**2 - q1**2 + q2**2 - q3**2, 2 * (q2 * q3 - q0 * q1)],
            [2 * (q1 * q3 - q0 * q2), 2 * (q2 * q3 + q0 * q1), q0**2 - q1**2 - q2**2 + q3**2],
        ]
    )

    return Rot


def quat_to_rot_simplified(q):
    q0 = q[0]
    q1 = q[1]
    q2 = q[2]
    q3 = q[3]

    # Use the simplified formula for unit quaternion to rotation matrix
    # as it produces a simpler and more stable EKF derivation given
    # the additional constraint: q0^2 + q1^2 + q2^2 + q3^2 = 1
    Rot = sf.Matrix(
        [
            [1 - 2 * q2**2 - 2 * q3**2, 2 * (q1 * q2 - q0 * q3), 2 * (q1 * q3 + q0 * q2)],
            [2 * (q1 * q2 + q0 * q3), 1 - 2 * q1**2 - 2 * q3**2, 2 * (q2 * q3 - q0 * q1)],
            [2 * (q1 * q3 - q0 * q2), 2 * (q2 * q3 + q0 * q1), 1 - 2 * q1**2 - 2 * q2**2],
        ]
    )

    return Rot


def quat_mult(p, q):
    r = sf.Matrix(
        [
            p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3],
            p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2],
            p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1],
            p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0],
        ]
    )

    return r


def sign_no_zero(x) -> sf.Scalar:
    """
    Returns -1 if x is negative, 1 if x is positive, and 1 if x is zero
    """
    return 2 * sf.Min(sf.sign(x), 0) + 1


def add_epsilon_sign(expr, var, eps):
    # Avoids a singularity at 0 while keeping the derivative correct
    return expr.subs(var, var + eps * sign_no_zero(var))


def generate_px4_function(function_name, output_names):
    from symforce.codegen import Codegen, CppConfig
    import os
    import fileinput

    codegen = Codegen.function(function_name, output_names=output_names, config=CppConfig())
    metadata = codegen.generate_function(output_dir="generated", skip_directory_nesting=True)

    for f in metadata.generated_files:
        print("  |- {}".format(os.path.relpath(f, metadata.output_dir)))

    # Replace cstdlib and Eigen functions by PX4 equivalents
    with fileinput.FileInput(os.path.abspath(metadata.generated_files[0]), inplace=True) as file:
        for line in file:
            line = line.replace("std::max", "math::max")
            line = line.replace("std::min", "math::min")
            line = line.replace("Eigen", "matrix")
            line = line.replace("matrix/Dense", "matrix/math.hpp")

            # don't allow underscore + uppercase identifier naming (always reserved for any use)
            line = re.sub(r"_([A-Z])", lambda x: "_" + x.group(1).lower(), line)

            print(line, end="")


def generate_python_function(function_name, output_names):
    from symforce.codegen import Codegen, PythonConfig

    codegen = Codegen.function(function_name, output_names=output_names, config=PythonConfig())

    metadata = codegen.generate_function(output_dir="generated", skip_directory_nesting=True)
