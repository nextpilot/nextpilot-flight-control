/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "comparison_helper.h"

float randf() {
    return (float)rand() / (float)RAND_MAX;
}

SquareMatrix24f createRandomCovarianceMatrix24f() {
    // Create a symmetric square matrix
    SquareMatrix24f P;

    for (int col = 0; col <= 23; col++) {
        for (int row = 0; row <= col; row++) {
            if (row == col) {
                P(row, col) = randf();

            } else {
                P(col, row) = P(row, col) = 2.0f * (randf() - 0.5f);
            }
        }
    }

    // Make it positive definite
    P = P.transpose() * P;

    return P;
}

DiffRatioReport computeDiffRatioVector24f(const Vector24f &v1, const Vector24f &v2) {
    DiffRatioReport report = {};

    for (int row = 0; row < 24; row++) {
        float diff_fraction;

        if (fabsf(v1(row)) > FLT_EPSILON) {
            diff_fraction = fabsf(v2(row) - v1(row)) / fabsf(v1(row));

        } else if (fabsf(v2(row)) > FLT_EPSILON) {
            diff_fraction = fabsf(v2(row) - v1(row)) / fabsf(v2(row));

        } else {
            diff_fraction = 0.0f;
        }

        if (diff_fraction > report.max_diff_fraction) {
            report.max_diff_fraction = diff_fraction;
            report.max_row           = row;
            report.max_v1            = v1(row);
            report.max_v2            = v2(row);
        }
    }

    return report;
}

DiffRatioReport computeDiffRatioSquareMatrix24f(const SquareMatrix24f &m1, const SquareMatrix24f &m2) {
    DiffRatioReport report = {};

    for (int row = 0; row < 24; row++) {
        for (int col = 0; col < 24; col++) {
            float diff_fraction;

            if (fabsf(m1(row, col)) > FLT_EPSILON) {
                diff_fraction = fabsf(m2(row, col) - m1(row, col)) / fabsf(m1(row, col));

            } else if (fabsf(m2(row, col)) > FLT_EPSILON) {
                diff_fraction = fabsf(m2(row, col) - m1(row, col)) / fabsf(m2(row, col));

            } else {
                diff_fraction = 0.0f;
            }

            if (diff_fraction > report.max_diff_fraction) {
                report.max_diff_fraction = diff_fraction;
                report.max_row           = row;
                report.max_v1            = m1(row, col);
                report.max_v2            = m2(row, col);
            }
        }
    }

    return report;
}
