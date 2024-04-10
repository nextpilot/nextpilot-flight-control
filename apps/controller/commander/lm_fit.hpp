/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <stdint.h>

#include <matrix/matrix/math.hpp>
#include <defines.h>

struct sphere_params {
    matrix::Vector3f offset, diag{1.f, 1.f, 1.f}, offdiag;
    float            radius{0.2f};
};

/**
 * Least-squares fit of a sphere to a set of points.
 *
 * Fits a sphere to a set of points on the sphere surface.
 *
 * @param x point coordinates on the X axis
 * @param y point coordinates on the Y axis
 * @param z point coordinates on the Z axis
 * @param samples_collected number of points
 * @param max_iterations abort if maximum number of iterations have been reached. If unsure, set to 100.
 * @param params the values to be optimized
 * @param full_ellipsoid whether to just optimize a sphere, or do an ellipsoid optimization
 *
 * NB!! If you optimize the full ellipsoid, you must have already optimized without the full ellipsoid
 *
 * @return 0 on success, 1 on failure
 */
int lm_mag_fit(const float x[], const float y[], const float z[], unsigned int samples_collected, sphere_params &params,
               bool full_ellipsoid);
