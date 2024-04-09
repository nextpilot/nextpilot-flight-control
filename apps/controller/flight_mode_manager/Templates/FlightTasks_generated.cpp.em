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
 * @@file FlightTasks_generated.cpp
 *
 * Generated file to switch between all required flight tasks
 *
 * @@author Christoph Tobler <christoph@@px4.io>
 */

#include "FlightModeManager.hpp"
#include "FlightTasks_generated.hpp"

int FlightModeManager::_initTask(FlightTaskIndex task_index)
{

	// disable the old task if there is any
	if (_current_task.task) {
		_current_task.task->~FlightTask();
		_current_task.task = nullptr;
		_current_task.index = FlightTaskIndex::None;
	}

	switch (task_index) {
	case FlightTaskIndex::None:
		// already disabled task
		break;

@# loop through all requested tasks
@[if tasks]@
@[for task in tasks]@
	case FlightTaskIndex::@(task):
		_current_task.task = new (&_task_union.@(task)) FlightTask@(task)();
		break;

@[end for]@
@[end if]@
	default:
		// invalid task
		return 1;
	}

	// task construction succeeded
	_current_task.index = task_index;
	return 0;
}
