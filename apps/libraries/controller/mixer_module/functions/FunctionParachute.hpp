/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include "FunctionProviderBase.hpp"

/**
 * Functions: Parachute
 */
class FunctionParachute : public FunctionProviderBase
{
public:
	FunctionParachute() = default;
	static FunctionProviderBase *allocate(const Context &context) { return new FunctionParachute(); }

	void update() override {}
	float value(OutputFunction func) override { return -1.f; }
	float defaultFailsafeValue(OutputFunction func) const override { return 1.f; }
};
