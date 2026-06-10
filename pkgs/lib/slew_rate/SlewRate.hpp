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
 * @file SlewRate.hpp
 *
 * Library limit the rate of change of a value with a maximum slew rate.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include <mathlib/mathlib.h>
#include <matrix/math.hpp>

template<typename Type>
class SlewRate
{
public:
	SlewRate() = default;
	~SlewRate() = default;

	/**
	 * Set maximum rate of change for the value
	 * @param slew_rate maximum rate of change
	 */
	void setSlewRate(const Type slew_rate) { _slew_rate = slew_rate; }

	/**
	 * Set value ignoring slew rate for initialization purpose
	 * @param value new applied value
	 */
	void setForcedValue(const Type value) { _value = value; }

	/**
	 * Get value from last update of the slew rate
	 * @return current value the slew rate is at
	 */
	Type getState() const { return _value; }

	/**
	 * Update slewrate
	 * @param new_value desired new value
	 * @param deltatime time in seconds since last update
	 * @return actual value that complies with the slew rate
	 */
	Type update(const Type new_value, const float deltatime)
	{
		// Limit the rate of change of the value
		const Type dvalue_desired = new_value - _value;
		const Type dvalue_max = _slew_rate * deltatime;
		const Type dvalue = math::constrain(dvalue_desired, -dvalue_max, dvalue_max);
		_value += dvalue;
		return _value;
	}

protected:
	Type _slew_rate{}; ///< maximum rate of change for the value
	Type _value{}; ///< state to keep last value of the slew rate
};
