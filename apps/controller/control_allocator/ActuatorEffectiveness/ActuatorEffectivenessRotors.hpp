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
 * @file ActuatorEffectivenessRotors.hpp
 *
 * Actuator effectiveness computed from rotors position and orientation
 *
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 */

#pragma once

#include "ActuatorEffectiveness.hpp"

#include <module_params.hpp>
#include <uORB/uORBSubscription.hpp>
#include <uORB/SubscriptionInterval.hpp>

class ActuatorEffectivenessTilts;

using namespace time_literals;

class ActuatorEffectivenessRotors : public ModuleParams, public ActuatorEffectiveness {
public:
    enum class AxisConfiguration {
        Configurable, ///< axis can be configured
        FixedForward, ///< axis is fixed, pointing forwards (positive X)
        FixedUpwards, ///< axis is fixed, pointing upwards (negative Z)
    };

    static constexpr int NUM_ROTORS_MAX = 12;

    struct RotorGeometry {
        matrix::Vector3f position;
        matrix::Vector3f axis;
        float            thrust_coef;
        float            moment_ratio;
        int              tilt_index;
    };

    struct Geometry {
        RotorGeometry rotors[NUM_ROTORS_MAX];
        int           num_rotors{0};
        bool          propeller_torque_disabled{false};
        bool          yaw_by_differential_thrust_disabled{false};
        bool          propeller_torque_disabled_non_upwards{false}; ///< keeps propeller torque enabled for upward facing motors
        bool          three_dimensional_thrust_disabled{false};     ///< for handling of tiltrotor VTOL, as they pass in 1D thrust and collective tilt
    };

    ActuatorEffectivenessRotors(ModuleParams *parent, AxisConfiguration axis_config = AxisConfiguration::Configurable,
                                bool tilt_support = false);
    virtual ~ActuatorEffectivenessRotors() = default;

    bool getEffectivenessMatrix(Configuration &configuration, EffectivenessUpdateReason external_update) override;

    void getDesiredAllocationMethod(AllocationMethod allocation_method_out[MAX_NUM_MATRICES]) const override {
        allocation_method_out[0] = AllocationMethod::SEQUENTIAL_DESATURATION;
    }

    void getNormalizeRPY(bool normalize[MAX_NUM_MATRICES]) const override {
        normalize[0] = true;
    }

    static int computeEffectivenessMatrix(const Geometry      &geometry,
                                          EffectivenessMatrix &effectiveness, int actuator_start_index = 0);

    bool addActuators(Configuration &configuration);

    const char *name() const override {
        return "Rotors";
    }

    /**
     * Sets the motor axis from tilt configurations and current tilt control.
     * @param tilts configured tilt servos
     * @param tilt_control current tilt control in [-1, 1] (can be NAN)
     * @return the motors as bitset which are not tiltable
     */
    uint32_t updateAxisFromTilts(const ActuatorEffectivenessTilts &tilts, float tilt_control);

    const Geometry &geometry() const {
        return _geometry;
    }

    /**
     * Get the tilted axis {0, 0, -1} rotated by -tilt_angle around y, then
     * rotated by tilt_direction around z.
     */
    static matrix::Vector3f tiltedAxis(float tilt_angle, float tilt_direction);

    void enablePropellerTorque(bool enable) {
        _geometry.propeller_torque_disabled = !enable;
    }

    void enableYawByDifferentialThrust(bool enable) {
        _geometry.yaw_by_differential_thrust_disabled = !enable;
    }

    void enablePropellerTorqueNonUpwards(bool enable) {
        _geometry.propeller_torque_disabled_non_upwards = !enable;
    }

    void enableThreeDimensionalThrust(bool enable) {
        _geometry.three_dimensional_thrust_disabled = !enable;
    }

    uint32_t getMotors() const;
    uint32_t getUpwardsMotors() const;
    uint32_t getForwardsMotors() const;

private:
    void                    updateParams() override;
    const AxisConfiguration _axis_config;
    const bool              _tilt_support; ///< if true, tilt servo assignment params are loaded

    struct ParamHandles {
        param_t position_x;
        param_t position_y;
        param_t position_z;
        param_t axis_x;
        param_t axis_y;
        param_t axis_z;
        param_t thrust_coef;
        param_t moment_ratio;
        param_t tilt_index;
    };
    ParamHandles _param_handles[NUM_ROTORS_MAX];
    param_t      _count_handle;

    Geometry _geometry{};
};
