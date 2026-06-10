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

#include <limits.h>
#include <float.h>
#include <mixer_module/output_functions.hpp>
#include <uORB/Subscription.hpp>
#include <workq/WorkItemScheduled.hpp>

using namespace nextpilot;
using namespace nextpilot::workq;

class FunctionProviderBase
{
public:
	struct Context {
		WorkItem &work_item;
		const float &thrust_factor;
	};

	FunctionProviderBase() = default;
	virtual ~FunctionProviderBase() = default;

	virtual void update() = 0;

	/**
	 * Get the current output value for a given function
	 * @return NAN (=disarmed) or value in range [-1, 1]
	 */
	virtual float value(OutputFunction func) = 0;

	virtual float defaultFailsafeValue(OutputFunction func) const { return NAN; }
	virtual bool allowPrearmControl() const { return true; }

	virtual uORB::SubscriptionCallbackWorkItem *subscriptionCallback() { return nullptr; }

	virtual bool getLatestSampleTimestamp(hrt_abstime &t) const { return false; }

	/**
	 * Check whether the output (motor) is configured to be reversible
	 */
	virtual bool reversible(OutputFunction func) const { return false; }
};
