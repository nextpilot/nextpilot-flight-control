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
 * Wrapper class for Ekf object to set behavior or check status
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_EKF_WRAPPER_H
#define EKF_EKF_WRAPPER_H

#include <memory>
#include "EKF/ekf.h"
#include "EKF/estimator_interface.h"

class EkfWrapper {
public:
    EkfWrapper(std::shared_ptr<Ekf> ekf);
    ~EkfWrapper();

    void setBaroHeightRef();
    void enableBaroHeightFusion();
    void disableBaroHeightFusion();
    bool isIntendingBaroHeightFusion() const;

    void setGpsHeightRef();
    void enableGpsHeightFusion();
    void disableGpsHeightFusion();
    bool isIntendingGpsHeightFusion() const;

    void setRangeHeightRef();
    void enableRangeHeightFusion();
    void disableRangeHeightFusion();
    bool isIntendingRangeHeightFusion() const;

    void setExternalVisionHeightRef();
    void enableExternalVisionHeightFusion();
    /* void disableExternalVisionHeightFusion(); */
    bool isIntendingExternalVisionHeightFusion() const;

    void enableBetaFusion();
    void disableBetaFusion();
    bool isIntendingBetaFusion() const;

    void enableGpsFusion();
    void disableGpsFusion();
    bool isIntendingGpsFusion() const;

    void enableGpsHeadingFusion();
    void disableGpsHeadingFusion();
    bool isIntendingGpsHeadingFusion() const;

    void enableFlowFusion();
    void disableFlowFusion();
    bool isIntendingFlowFusion() const;
    void setFlowOffset(const matrix::Vector3f &offset);

    void enableExternalVisionPositionFusion();
    void disableExternalVisionPositionFusion();
    bool isIntendingExternalVisionPositionFusion() const;

    void enableExternalVisionVelocityFusion();
    void disableExternalVisionVelocityFusion();
    bool isIntendingExternalVisionVelocityFusion() const;

    void enableExternalVisionHeadingFusion();
    void disableExternalVisionHeadingFusion();
    bool isIntendingExternalVisionHeadingFusion() const;

    bool isIntendingMagHeadingFusion() const;
    bool isIntendingMag3DFusion() const;
    void setMagFuseTypeNone();
    void enableMagStrengthCheck();

    bool isWindVelocityEstimated() const;

    void enableTerrainRngFusion();
    void disableTerrainRngFusion();
    bool isIntendingTerrainRngFusion() const;

    void enableTerrainFlowFusion();
    void disableTerrainFlowFusion();
    bool isIntendingTerrainFlowFusion() const;

    Eulerf           getEulerAngles() const;
    float            getYawAngle() const;
    matrix::Vector4f getQuaternionVariance() const;
    int              getQuaternionResetCounter() const;

    matrix::Vector3f getDeltaVelBiasVariance() const;

    void enableDragFusion();
    void disableDragFusion();
    void setDragFusionParameters(const float &bcoef_x, const float &bcoef_y, const float &mcoef);

private:
    std::shared_ptr<Ekf> _ekf;

    // Pointer to Ekf internal param struct
    parameters *_ekf_params;
};
#endif // !EKF_EKF_WRAPPER_H
