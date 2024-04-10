/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "ActuatorEffectivenessTilts.hpp"

// #include <px4_platform_common/log.h>
#include <mathlib/mathlib.h>
#include <rtdbg.h>

using namespace matrix;

ActuatorEffectivenessTilts::ActuatorEffectivenessTilts(ModuleParams *parent) :
    ModuleParams(parent) {
    for (int i = 0; i < MAX_COUNT; ++i) {
        char buffer[17];
        snprintf(buffer, sizeof(buffer), "CA_SV_TL%u_CT", i);
        _param_handles[i].control = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_TL%u_MINA", i);
        _param_handles[i].min_angle = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_TL%u_MAXA", i);
        _param_handles[i].max_angle = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_TL%u_TD", i);
        _param_handles[i].tilt_direction = param_find(buffer);
    }

    _count_handle = param_find("CA_SV_TL_COUNT");
    updateParams();
}

void ActuatorEffectivenessTilts::updateParams() {
    ModuleParams::updateParams();

    int32_t count = 0;

    if (param_get(_count_handle, &count) != 0) {
        PX4_ERR("param_get failed");
        return;
    }

    _count = count;

    for (int i = 0; i < count; i++) {
        param_get(_param_handles[i].control, (int32_t *)&_params[i].control);
        param_get(_param_handles[i].tilt_direction, (int32_t *)&_params[i].tilt_direction);
        param_get(_param_handles[i].min_angle, &_params[i].min_angle);
        param_get(_param_handles[i].max_angle, &_params[i].max_angle);

        _params[i].min_angle = math::radians(_params[i].min_angle);
        _params[i].max_angle = math::radians(_params[i].max_angle);

        _torque[i].setZero();
    }
}

bool ActuatorEffectivenessTilts::addActuators(Configuration &configuration) {
    for (int i = 0; i < _count; i++) {
        configuration.addActuator(ActuatorType::SERVOS, _torque[i], Vector3f{});
    }

    return true;
}

void ActuatorEffectivenessTilts::updateTorqueSign(const ActuatorEffectivenessRotors::Geometry &geometry,
                                                  bool                                         disable_pitch) {
    for (int i = 0; i < geometry.num_rotors; ++i) {
        int tilt_index = geometry.rotors[i].tilt_index;

        if (tilt_index == -1 || tilt_index >= _count) {
            continue;
        }

        if (_params[tilt_index].control == Control::Yaw || _params[tilt_index].control == Control::YawAndPitch) {
            // Find the yaw torque sign by checking the motor position and tilt direction.
            // Rotate position by -tilt_direction around z, then check the sign of y pos
            float    tilt_direction = math::radians((float)_params[tilt_index].tilt_direction);
            Vector3f rotated_pos    = Dcmf{Eulerf{0.f, 0.f, -tilt_direction}} * geometry.rotors[i].position;

            if (rotated_pos(1) < -0.01f) { // add minimal margin
                _torque[tilt_index](2) = 1.f;

            } else if (rotated_pos(1) > 0.01f) {
                _torque[tilt_index](2) = -1.f;
            }
        }

        if (!disable_pitch && (_params[tilt_index].control == Control::Pitch || _params[tilt_index].control == Control::YawAndPitch)) {
            bool tilting_forwards  = (int)_params[tilt_index].tilt_direction < 90 || (int)_params[tilt_index].tilt_direction > 270;
            _torque[tilt_index](1) = tilting_forwards ? -1.f : 1.f;
        }
    }
}

bool ActuatorEffectivenessTilts::hasYawControl() const {
    for (int i = 0; i < _count; i++) {
        if (_params[i].control == Control::Yaw || _params[i].control == Control::YawAndPitch) {
            return true;
        }
    }

    return false;
}
