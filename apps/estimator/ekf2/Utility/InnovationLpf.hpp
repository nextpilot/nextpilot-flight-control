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
 * First order "alpha" IIR digital filter with input saturation
 */

#include <mathlib/mathlib.h>

class InnovationLpf final {
public:
    InnovationLpf()  = default;
    ~InnovationLpf() = default;

    void reset(float val = 0.f) {
        _x = val;
    }

    /**
     * Update the filter with a new value and returns the filtered state
     * The new value is constained by the limit set in setSpikeLimit
     * @param val new input
     * @param alpha normalized weight of the new input
     * @param spike_limit the amplitude of the saturation at the input of the filter
     * @return filtered output
     */
    float update(float val, float alpha, float spike_limit) {
        float val_constrained = math::constrain(val, -spike_limit, spike_limit);
        float beta            = 1.f - alpha;

        _x = beta * _x + alpha * val_constrained;

        return _x;
    }

    /**
     * Helper function to compute alpha from dt and the inverse of tau
     * @param dt sampling time in seconds
     * @param tau_inv inverse of the time constant of the filter
     * @return alpha, the normalized weight of a new measurement
     */
    static float computeAlphaFromDtAndTauInv(float dt, float tau_inv) {
        return math::constrain(dt * tau_inv, 0.f, 1.f);
    }

private:
    float _x{}; ///< current state of the filter
};
