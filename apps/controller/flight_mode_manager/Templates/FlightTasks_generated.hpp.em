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
 * @@file FlightTasks_generated.hpp
 *
 * Generated Header to list all required flight tasks
 *
 * @@author Christoph Tobler <christoph@@px4.io>
 */

 #pragma once

// include all required headers
#include "FlightModeManager.hpp"
@# loop through all requested tasks
@[if tasks]@
@[for task in tasks]@
#include "FlightTask@(task).hpp"
@[end for]@
@[end if]@

enum class FlightTaskIndex : int {
    None = -1,
@# loop through all requested tasks
@[if tasks]@
@[for task in tasks]@
    @(task),
@[end for]@
@[end if]@

    Count // number of tasks
};

union TaskUnion {
    TaskUnion() {}
    ~TaskUnion() {}

@# loop through all requested tasks
@[if tasks]@
@[for task in tasks]@
    FlightTask@(task) @(task);
@[end for]@
@[end if]@
};
