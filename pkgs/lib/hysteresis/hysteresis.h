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
 * @file hysteresis.h
 *
 * Hysteresis of a boolean value.
 *
 * @author Julian Oes <julian@oes.ch>
 */

#pragma once

#include <hrtimer.h>

namespace nextpilot
{

class Hysteresis
{
public:
	explicit Hysteresis(bool init_state) :
		_state(init_state),
		_requested_state(init_state)
	{}
	Hysteresis() = delete; // no default constructor

	~Hysteresis() = default;

	bool get_state() const { return _state; }

	void set_hysteresis_time_from(const bool from_state, const hrt_abstime new_hysteresis_time_us);

	void set_state_and_update(const bool new_state, const hrt_abstime &now_us);

	void update(const hrt_abstime &now_us);

private:

	hrt_abstime _last_time_to_change_state{0};

	hrt_abstime _time_from_true_us{0};
	hrt_abstime _time_from_false_us{0};

	bool _state;
	bool _requested_state;
};

} // namespace nextpilot
