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
 * @file AirshipLandDetector.h
 * Land detection implementation for airships.
 *
 * @author Anton Erasmus <anton@flycloudline.com>
 */

#pragma once

#include "LandDetector.h"

namespace land_detector {

class AirshipLandDetector : public LandDetector {
public:
    AirshipLandDetector()           = default;
    ~AirshipLandDetector() override = default;

protected:
    bool _get_ground_contact_state() override;
    bool _get_landed_state() override;
    void _set_hysteresis_factor(const int factor) override {};
};

} // namespace land_detector
