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
 * @file loiter.h
 *
 * Helper class to loiter
 *
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include "navigator_mode.h"
#include "mission_block.h"
#include <module_params.hpp>

class Loiter : public MissionBlock, public ModuleParams {
public:
    Loiter(Navigator *navigator);
    ~Loiter() = default;

    void on_inactive() override;
    void on_activation() override;
    void on_active() override;

private:
    /**
     * Use the stored reposition location of the navigator
     * to move to a new location.
     */
    void reposition();

    /**
     * Set the position to hold based on the current local position
     */
    void set_loiter_position();

    bool _loiter_pos_set{false};
};
