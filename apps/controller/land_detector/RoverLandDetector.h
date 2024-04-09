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
 * @file RoverLandDetector.h
 * Land detection implementation for VTOL also called hybrids.
 *
 * @author Roman Bapst <bapstr@gmail.com>
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include "LandDetector.h"

namespace land_detector {

class RoverLandDetector : public LandDetector {
public:
    RoverLandDetector()           = default;
    ~RoverLandDetector() override = default;

protected:
    bool _get_ground_contact_state() override;
    bool _get_landed_state() override;
    void _set_hysteresis_factor(const int factor) override {};
};

} // namespace land_detector
