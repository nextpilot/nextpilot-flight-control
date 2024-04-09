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
 * @file takeoff.h
 *
 * Helper class to take off
 *
 * @author Lorenz Meier <lorenz@px4.io>
 */

#pragma once

#include "navigator_mode.h"
#include "mission_block.h"

class Takeoff : public MissionBlock {
public:
    Takeoff(Navigator *navigator);
    ~Takeoff() = default;

    void on_activation() override;
    void on_active() override;

private:
    void set_takeoff_position();
};
