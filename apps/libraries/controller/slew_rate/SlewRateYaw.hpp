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
 * @file SlewRateYaw.hpp
 *
 * Library limit the rate of change of a [-pi,pi] range yaw value with a maximum slew rate.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include "SlewRate.hpp"

template<typename Type>
class SlewRateYaw : public SlewRate<Type>
{
public:
	SlewRateYaw() = default;
	~SlewRateYaw() = default;

	/**
	 * Update slewrate with yaw wrapping [-pi,pi]
	 * @param new_value desired new value
	 * @param deltatime time in seconds since last update
	 * @return actual value that complies with the slew rate
	 */
	Type update(const Type new_value, const float deltatime)
	{
		const Type d_wrapped = matrix::wrap_pi(new_value - this->_value);
		return matrix::wrap_pi(SlewRate<Type>::update(this->_value + d_wrapped, deltatime));
	}
};
