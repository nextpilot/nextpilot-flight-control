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
 * @file VtolLandDetector.h
 * Land detection implementation for VTOLs.
 * It uses the MC land detector in hover, while land detection in FW
 * is 1:1 linked to the (boolean) armed state.
 *
 * @author Roman Bapst <bapstr@gmail.com>
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include "MulticopterLandDetector.h"

namespace land_detector {

class VtolLandDetector : public MulticopterLandDetector {
public:
    VtolLandDetector()           = default;
    ~VtolLandDetector() override = default;

protected:
    void _update_topics() override;
    bool _get_landed_state() override;
    bool _get_maybe_landed_state() override;
    bool _get_freefall_state() override;
};

} // namespace land_detector
