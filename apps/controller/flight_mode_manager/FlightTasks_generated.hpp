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
 * @file FlightTasks_generated.hpp
 *
 * Generated Header to list all required flight tasks
 *
 * @author Christoph Tobler <christoph@px4.io>
 */

 #pragma once

// include all required headers
#include "FlightModeManager.hpp"
#include "FlightTaskAuto.hpp"
#include "FlightTaskDescend.hpp"
#include "FlightTaskFailsafe.hpp"
#include "FlightTaskManualAcceleration.hpp"
#include "FlightTaskManualAltitude.hpp"
#include "FlightTaskManualAltitudeSmoothVel.hpp"
#include "FlightTaskManualPosition.hpp"
#include "FlightTaskManualPositionSmoothVel.hpp"
#include "FlightTaskTransition.hpp"
#include "FlightTaskAutoFollowTarget.hpp"
#include "FlightTaskOrbit.hpp"

enum class FlightTaskIndex : int {
    None = -1,
    Auto,
    Descend,
    Failsafe,
    ManualAcceleration,
    ManualAltitude,
    ManualAltitudeSmoothVel,
    ManualPosition,
    ManualPositionSmoothVel,
    Transition,
    AutoFollowTarget,
    Orbit,

    Count // number of tasks
};

union TaskUnion {
    TaskUnion() {}
    ~TaskUnion() {}

    FlightTaskAuto Auto;
    FlightTaskDescend Descend;
    FlightTaskFailsafe Failsafe;
    FlightTaskManualAcceleration ManualAcceleration;
    FlightTaskManualAltitude ManualAltitude;
    FlightTaskManualAltitudeSmoothVel ManualAltitudeSmoothVel;
    FlightTaskManualPosition ManualPosition;
    FlightTaskManualPositionSmoothVel ManualPositionSmoothVel;
    FlightTaskTransition Transition;
    FlightTaskAutoFollowTarget AutoFollowTarget;
    FlightTaskOrbit Orbit;
};
