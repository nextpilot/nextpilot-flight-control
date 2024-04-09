/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef EKF_COMPARISON_HELPER
#define EKF_COMPARISON_HELPER

#include "EKF/ekf.h"

typedef matrix::Vector<float, 24>       Vector24f;
typedef matrix::SquareMatrix<float, 24> SquareMatrix24f;
template <int... Idxs>
using SparseVector24f = matrix::SparseVectorf<24, Idxs...>;

struct DiffRatioReport {
    float max_diff_fraction;
    float max_row;
    float max_v1;
    float max_v2;
};

float randf();

// Create a symmetrical positive dfinite matrix with off diagonals between -1 and 1 and diagonals between 0 and 1
SquareMatrix24f createRandomCovarianceMatrix24f();

// Find largest element-wise difference as a fraction of v1 or v2
DiffRatioReport computeDiffRatioVector24f(const Vector24f &v1, const Vector24f &v2);
DiffRatioReport computeDiffRatioSquareMatrix24f(const SquareMatrix24f &m1, const SquareMatrix24f &m2);
#endif
