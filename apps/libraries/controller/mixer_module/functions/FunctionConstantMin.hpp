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
 * Functions: Constant_Min
 */
class FunctionConstantMin : public FunctionProviderBase
{
public:
	static FunctionProviderBase *allocate(const Context &context) { return new FunctionConstantMin(); }

	float value(OutputFunction func) override { return -1.f; }
	void update() override { }

	float defaultFailsafeValue(OutputFunction func) const override { return -1.f; }
};
