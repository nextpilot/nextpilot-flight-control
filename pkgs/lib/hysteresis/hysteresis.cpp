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
 * @file hysteresis.cpp
 *
 * @author Julian Oes <julian@oes.ch>
 */

#include "hysteresis.h"

namespace nextpilot
{

void
Hysteresis::set_hysteresis_time_from(const bool from_state, const hrt_abstime new_hysteresis_time_us)
{
	if (from_state) {
		_time_from_true_us = new_hysteresis_time_us;

	} else {
		_time_from_false_us = new_hysteresis_time_us;
	}
}

void
Hysteresis::set_state_and_update(const bool new_state, const hrt_abstime &now_us)
{
	if (new_state != _state) {
		if (new_state != _requested_state) {
			_requested_state = new_state;
			_last_time_to_change_state = now_us;
		}

	} else {
		_requested_state = _state;
	}

	update(now_us);
}

void
Hysteresis::update(const hrt_abstime &now_us)
{
	if (_requested_state != _state) {

		if (_state && !_requested_state) {
			// true -> false
			if (now_us >= _last_time_to_change_state + _time_from_true_us) {
				_state = false;
			}

		} else if (!_state && _requested_state) {
			// false -> true
			if (now_us >= _last_time_to_change_state + _time_from_false_us) {
				_state = true;
			}
		}
	}
}

} // namespace nextpilot
