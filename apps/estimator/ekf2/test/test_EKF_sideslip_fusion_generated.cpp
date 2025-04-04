/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <gtest/gtest.h>
#include "EKF/ekf.h"
#include "test_helper/comparison_helper.h"

#include "../EKF/python/ekf_derivation/generated/compute_sideslip_innov_and_innov_var.h"
#include "../EKF/python/ekf_derivation/generated/compute_sideslip_h_and_k.h"

using namespace matrix;

TEST(SideslipFusionGenerated, SympyVsSymforce) {
    // Compare calculation of observation Jacobians and Kalman gains for sympy and symforce generated equations
    const float R_BETA = sq(2.5f);

    const Quatf q(Eulerf(-M_PI_F / 2.f, M_PI_F / 3.f, M_PI_F * 4.f / 5.f));
    const float q0 = q(0);
    const float q1 = q(1);
    const float q2 = q(2);
    const float q3 = q(3);

    const float vn = 9.0f;
    const float ve = 12.0f;
    const float vd = -1.5f;

    const float vwn = -4.0f;
    const float vwe = 3.0f;

    SquareMatrix24f P = createRandomCovarianceMatrix24f();

    // First calculate observationjacobians and Kalman gains using sympy generated equations
    Vector24f Hfusion_sympy;
    Vector24f Kfusion_sympy;

    {
        // Intermediate Values
        const float HK0  = vn - vwn;
        const float HK1  = ve - vwe;
        const float HK2  = HK0 * q0 + HK1 * q3 - q2 * vd;
        const float HK3  = q0 * q2 - q1 * q3;
        const float HK4  = 2 * vd;
        const float HK5  = q0 * q3;
        const float HK6  = q1 * q2;
        const float HK7  = 2 * HK5 + 2 * HK6;
        const float HK8  = ecl::powf(q0, 2);
        const float HK9  = ecl::powf(q3, 2);
        const float HK10 = HK8 - HK9;
        const float HK11 = ecl::powf(q1, 2);
        const float HK12 = ecl::powf(q2, 2);
        const float HK13 = HK11 - HK12;
        const float HK14 = HK10 + HK13;
        const float HK15 = HK0 * HK14 + HK1 * HK7 - HK3 * HK4;
        const float HK16 = 1.0F / HK15;
        const float HK17 = q0 * q1 + q2 * q3;
        const float HK18 = HK10 - HK11 + HK12;
        const float HK19 = HK16 * (-2 * HK0 * (HK5 - HK6) + HK1 * HK18 + HK17 * HK4);
        const float HK20 = -HK0 * q3 + HK1 * q0 + q1 * vd;
        const float HK21 = -HK19 * HK2 + HK20;
        const float HK22 = 2 * HK16;
        const float HK23 = HK0 * q1 + HK1 * q2 + q3 * vd;
        const float HK24 = HK0 * q2 - HK1 * q1 + q0 * vd;
        const float HK25 = -HK19 * HK23 + HK24;
        const float HK26 = HK19 * HK24 + HK23;
        const float HK27 = HK19 * HK20 + HK2;
        const float HK28 = HK14 * HK19 + 2 * HK5 - 2 * HK6;
        const float HK29 = HK16 * HK28;
        const float HK30 = HK19 * HK7;
        const float HK31 = HK17 + HK19 * HK3;
        const float HK32 = HK13 + HK30 - HK8 + HK9;
        const float HK33 = 2 * HK31;
        const float HK34 = 2 * HK26;
        const float HK35 = 2 * HK25;
        const float HK36 = 2 * HK27;
        const float HK37 = 2 * HK21;
        const float HK38 = HK28 * P(0, 22) - HK28 * P(0, 4) + HK32 * P(0, 23) - HK32 * P(0, 5) + HK33 * P(0, 6) + HK34 * P(0, 2) + HK35 * P(0, 1) - HK36 * P(0, 3) + HK37 * P(0, 0);
        const float HK39 = ecl::powf(HK15, -2);
        const float HK40 = -HK28 * P(4, 6) + HK28 * P(6, 22) - HK32 * P(5, 6) + HK32 * P(6, 23) + HK33 * P(6, 6) + HK34 * P(2, 6) + HK35 * P(1, 6) - HK36 * P(3, 6) + HK37 * P(0, 6);
        const float HK41 = HK32 * P(5, 23);
        const float HK42 = HK28 * P(22, 23) - HK28 * P(4, 23) + HK32 * P(23, 23) + HK33 * P(6, 23) + HK34 * P(2, 23) + HK35 * P(1, 23) - HK36 * P(3, 23) + HK37 * P(0, 23) - HK41;
        const float HK43 = HK32 * HK39;
        const float HK44 = HK28 * P(4, 22);
        const float HK45 = HK28 * P(22, 22) + HK32 * P(22, 23) - HK32 * P(5, 22) + HK33 * P(6, 22) + HK34 * P(2, 22) + HK35 * P(1, 22) - HK36 * P(3, 22) + HK37 * P(0, 22) - HK44;
        const float HK46 = HK28 * HK39;
        const float HK47 = -HK28 * P(4, 5) + HK28 * P(5, 22) - HK32 * P(5, 5) + HK33 * P(5, 6) + HK34 * P(2, 5) + HK35 * P(1, 5) - HK36 * P(3, 5) + HK37 * P(0, 5) + HK41;
        const float HK48 = -HK28 * P(4, 4) + HK32 * P(4, 23) - HK32 * P(4, 5) + HK33 * P(4, 6) + HK34 * P(2, 4) + HK35 * P(1, 4) - HK36 * P(3, 4) + HK37 * P(0, 4) + HK44;
        const float HK49 = HK28 * P(2, 22) - HK28 * P(2, 4) + HK32 * P(2, 23) - HK32 * P(2, 5) + HK33 * P(2, 6) + HK34 * P(2, 2) + HK35 * P(1, 2) - HK36 * P(2, 3) + HK37 * P(0, 2);
        const float HK50 = HK28 * P(1, 22) - HK28 * P(1, 4) + HK32 * P(1, 23) - HK32 * P(1, 5) + HK33 * P(1, 6) + HK34 * P(1, 2) + HK35 * P(1, 1) - HK36 * P(1, 3) + HK37 * P(0, 1);
        const float HK51 = HK28 * P(3, 22) - HK28 * P(3, 4) + HK32 * P(3, 23) - HK32 * P(3, 5) + HK33 * P(3, 6) + HK34 * P(2, 3) + HK35 * P(1, 3) - HK36 * P(3, 3) + HK37 * P(0, 3);
        // const float HK52 = HK16/(HK33*HK39*HK40 + HK34*HK39*HK49 + HK35*HK39*HK50 - HK36*HK39*HK51 + HK37*HK38*HK39 + HK42*HK43 - HK43*HK47 + HK45*HK46 - HK46*HK48 + R_BETA);

        // innovation variance
        float _beta_innov_var = (HK33 * HK39 * HK40 + HK34 * HK39 * HK49 + HK35 * HK39 * HK50 - HK36 * HK39 * HK51 + HK37 * HK38 * HK39 + HK42 * HK43 - HK43 * HK47 + HK45 * HK46 - HK46 * HK48 + R_BETA);

        const float HK52 = HK16 / _beta_innov_var;

        // Observation Jacobians
        SparseVector24f<0, 1, 2, 3, 4, 5, 6, 22, 23> Hfusion;
        Hfusion.at<0>()  = HK21 * HK22;
        Hfusion.at<1>()  = HK22 * HK25;
        Hfusion.at<2>()  = HK22 * HK26;
        Hfusion.at<3>()  = -HK22 * HK27;
        Hfusion.at<4>()  = -HK29;
        Hfusion.at<5>()  = HK16 * (HK18 - HK30);
        Hfusion.at<6>()  = HK22 * HK31;
        Hfusion.at<22>() = HK29;
        Hfusion.at<23>() = HK16 * HK32;

        // Calculate Kalman gains
        Vector24f Kfusion;

        bool update_wind_only = false;

        if (!update_wind_only) {
            Kfusion(0) = HK38 * HK52;
            Kfusion(1) = HK50 * HK52;
            Kfusion(2) = HK49 * HK52;
            Kfusion(3) = HK51 * HK52;
            Kfusion(4) = HK48 * HK52;
            Kfusion(5) = HK47 * HK52;
            Kfusion(6) = HK40 * HK52;

            for (unsigned row = 7; row <= 21; row++) {
                Kfusion(row) = HK52 * (HK28 * P(row, 22) - HK28 * P(4, row) + HK32 * P(row, 23) - HK32 * P(5, row) + HK33 * P(6, row) + HK34 * P(2, row) + HK35 * P(1, row) - HK36 * P(3, row) + HK37 * P(0, row));
            }
        }

        Kfusion(22) = HK45 * HK52;
        Kfusion(23) = HK42 * HK52;

        Hfusion_sympy(0)  = Hfusion.at<0>();
        Hfusion_sympy(1)  = Hfusion.at<1>();
        Hfusion_sympy(2)  = Hfusion.at<2>();
        Hfusion_sympy(3)  = Hfusion.at<3>();
        Hfusion_sympy(4)  = Hfusion.at<4>();
        Hfusion_sympy(5)  = Hfusion.at<5>();
        Hfusion_sympy(6)  = Hfusion.at<6>();
        Hfusion_sympy(22) = Hfusion.at<22>();
        Hfusion_sympy(23) = Hfusion.at<23>();
        Kfusion_sympy     = Kfusion;
    }

    // Then calculate observationjacobians and Kalman gains using symforce generated equations
    Vector24f Hfusion_symforce;
    Vector24f Kfusion_symforce;

    {
        Vector24f state_vector{};
        state_vector(0)  = q0;
        state_vector(1)  = q1;
        state_vector(2)  = q2;
        state_vector(3)  = q3;
        state_vector(4)  = vn;
        state_vector(5)  = ve;
        state_vector(6)  = vd;
        state_vector(22) = vwn;
        state_vector(23) = vwe;

        float innov;
        float innov_var;

        sym::ComputeSideslipInnovAndInnovVar(state_vector, P, R_BETA, FLT_EPSILON, &innov, &innov_var);
        sym::ComputeSideslipHAndK(state_vector, P, innov_var, FLT_EPSILON, &Hfusion_symforce, &Kfusion_symforce);
    }

    DiffRatioReport report = computeDiffRatioVector24f(Hfusion_sympy, Hfusion_symforce);
    EXPECT_LT(report.max_diff_fraction, 1e-5f) << "Hfusion max diff fraction = " << report.max_diff_fraction << " location index = " << report.max_row << " sympy = " << report.max_v1 << " symforce = " << report.max_v2;

    report = computeDiffRatioVector24f(Kfusion_sympy, Kfusion_symforce);
    EXPECT_LT(report.max_diff_fraction, 1e-5f) << "Kfusion max diff fraction = " << report.max_diff_fraction << " location index = " << report.max_row << " sympy = " << report.max_v1 << " symforce = " << report.max_v2;
}
