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
 * @file mission_feasibility_checker.h
 * Provides checks if mission is feasible given the navigation capabilities
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author Thomas Gubler <thomasgubler@student.ethz.ch>
 * @author Sander Smeets <sander@droneslab.com>
 * @author Nuno Marques <nuno.marques@dronesolutions.io>
 */

#pragma once

#include <dataman/dataman.h>
#include <uORB/topics/mission.h>
#include <px4_platform_common/module_params.h>
#include "MissionFeasibility/FeasibilityChecker.hpp"

class Geofence;
class Navigator;

class MissionFeasibilityChecker : public ModuleParams {
private:
    Navigator         *_navigator{nullptr};
    FeasibilityChecker _feasibility_checker;

    bool checkGeofence(const mission_s &mission, float home_alt, bool home_valid);

public:
    MissionFeasibilityChecker(Navigator *navigator) :
        ModuleParams(nullptr),
        _navigator(navigator),
        _feasibility_checker() {
    }
    ~MissionFeasibilityChecker() = default;

    MissionFeasibilityChecker(const MissionFeasibilityChecker &)            = delete;
    MissionFeasibilityChecker &operator=(const MissionFeasibilityChecker &) = delete;

    /*
     * Returns true if mission is feasible and false otherwise
     */
    bool checkMissionFeasible(const mission_s &mission);
};
