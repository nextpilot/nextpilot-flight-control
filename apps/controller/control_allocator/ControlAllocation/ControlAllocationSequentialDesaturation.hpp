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
 * @file ControlAllocationSequentialDesaturation.hpp
 *
 * Control Allocation Algorithm which sequentially modifies control demands in order to
 * eliminate the saturation of the actuator setpoint vector.
 *
 *
 * @author Roman Bapst <bapstroman@gmail.com>
 */

#pragma once

#include "ControlAllocationPseudoInverse.hpp"
#include <module/module_params.hpp>

using namespace nextpilot::param;

class ControlAllocationSequentialDesaturation : public ControlAllocationPseudoInverse, public ModuleParams {
public:
    ControlAllocationSequentialDesaturation() :
        ModuleParams(nullptr) {
    }

    virtual ~ControlAllocationSequentialDesaturation() = default;

    void allocate() override;

    void updateParameters() override;

private:
    /**
     * Minimize the saturation of the actuators by adding or substracting a fraction of desaturation_vector.
     * desaturation_vector is the vector that added to the output outputs, modifies the thrust or angular
     * acceleration on a specific axis.
     * For example, if desaturation_vector is given to slide along the vertical thrust axis (thrust_scale), the
     * saturation will be minimized by shifting the vertical thrust setpoint, without changing the
     * roll/pitch/yaw accelerations.
     *
     * Note that as we only slide along the given axis, in extreme cases outputs can still contain values
     * outside of [min_output, max_output].
     *
     * @param actuator_sp Actuator setpoint, vector that is modified
     * @param desaturation_vector vector that is added to the outputs, e.g. thrust_scale
     * @param increase_only if true, only allow to increase (add) a fraction of desaturation_vector
     */
    void desaturateActuators(ActuatorVector &actuator_sp, const ActuatorVector &desaturation_vector,
                             bool increase_only = false);

    /**
     * Computes the gain k by which desaturation_vector has to be multiplied
     * in order to unsaturate the output that has the greatest saturation.
     *
     * @return desaturation gain
     */
    float computeDesaturationGain(const ActuatorVector &desaturation_vector, const ActuatorVector &actuator_sp);

    /**
     * Mix roll, pitch, yaw, thrust and set the actuator setpoint.
     *
     * Desaturation behavior: airmode for roll/pitch:
     * thrust is increased/decreased as much as required to meet the demanded roll/pitch.
     * Yaw is not allowed to increase the thrust, @see mix_yaw() for the exact behavior.
     */
    void mixAirmodeRP();

    /**
     * Mix roll, pitch, yaw, thrust and set the actuator setpoint.
     *
     * Desaturation behavior: full airmode for roll/pitch/yaw:
     * thrust is increased/decreased as much as required to meet demanded the roll/pitch/yaw,
     * while giving priority to roll and pitch over yaw.
     */
    void mixAirmodeRPY();

    /**
     * Mix roll, pitch, yaw, thrust and set the actuator setpoint.
     *
     * Desaturation behavior: no airmode, thrust is NEVER increased to meet the demanded
     * roll/pitch/yaw. Instead roll/pitch/yaw is reduced as much as needed.
     * Thrust can be reduced to unsaturate the upper side.
     * @see mixYaw() for the exact yaw behavior.
     */
    void mixAirmodeDisabled();

    /**
     * Mix yaw by updating the actuator setpoint (that already contains roll/pitch/thrust).
     *
     * Desaturation behavior: thrust is allowed to be decreased up to 15% in order to allow
     * some yaw control on the upper end. On the lower end thrust will never be increased,
     * but yaw is decreased as much as required.
     */
    void mixYaw();

    DEFINE_PARAMETERS(
        (ParamInt<params_id::MC_AIRMODE>)_param_mc_airmode ///< air-mode
    );
};
