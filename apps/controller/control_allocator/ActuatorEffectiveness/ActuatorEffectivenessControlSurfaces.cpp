/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <px4_platform_common/log.h>

#include "ActuatorEffectivenessControlSurfaces.hpp"

using namespace matrix;

ActuatorEffectivenessControlSurfaces::ActuatorEffectivenessControlSurfaces(ModuleParams *parent) :
    ModuleParams(parent) {
    for (int i = 0; i < MAX_COUNT; ++i) {
        char buffer[17];
        snprintf(buffer, sizeof(buffer), "CA_SV_CS%u_TYPE", i);
        _param_handles[i].type = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_CS%u_TRQ_R", i);
        _param_handles[i].torque[0] = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_CS%u_TRQ_P", i);
        _param_handles[i].torque[1] = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_CS%u_TRQ_Y", i);
        _param_handles[i].torque[2] = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_CS%u_TRIM", i);
        _param_handles[i].trim = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_CS%u_FLAP", i);
        _param_handles[i].scale_flap = param_find(buffer);
        snprintf(buffer, sizeof(buffer), "CA_SV_CS%u_SPOIL", i);
        _param_handles[i].scale_spoiler = param_find(buffer);
    }

    _flaps_setpoint_with_slewrate.setSlewRate(kFlapSlewRate);
    _spoilers_setpoint_with_slewrate.setSlewRate(kSpoilersSlewRate);

    _count_handle = param_find("CA_SV_CS_COUNT");
    updateParams();
}

void ActuatorEffectivenessControlSurfaces::updateParams() {
    ModuleParams::updateParams();

    int32_t count = 0;

    if (param_get(_count_handle, &count) != 0) {
        PX4_ERR("param_get failed");
        return;
    }

    _count = count;

    for (int i = 0; i < _count; i++) {
        param_get(_param_handles[i].type, (int32_t *)&_params[i].type);

        Vector3f &torque = _params[i].torque;

        for (int n = 0; n < 3; ++n) {
            param_get(_param_handles[i].torque[n], &torque(n));
        }

        param_get(_param_handles[i].trim, &_params[i].trim);
        param_get(_param_handles[i].scale_flap, &_params[i].scale_flap);
        param_get(_param_handles[i].scale_spoiler, &_params[i].scale_spoiler);

        // TODO: enforce limits (note that tailsitter uses different limits)?
        switch (_params[i].type) {
        case Type::LeftAileron:
            break;

        case Type::RightAileron:
            break;

        case Type::Elevator:
            break;

        case Type::Rudder:
            break;

        case Type::LeftElevon:
            break;

        case Type::RightElevon:
            break;

        case Type::LeftVTail:
            break;

        case Type::RightVTail:
            break;

        case Type::LeftFlap:
        case Type::RightFlap:
            torque.setZero();
            break;

        case Type::Airbrake:
            torque.setZero();
            break;

        case Type::Custom:
            break;

        case Type::LeftATail:
            break;

        case Type::RightATail:
            break;

        case Type::SingleChannelAileron:
            break;

        case Type::SteeringWheel:
            torque.setZero();
            break;

        case Type::LeftSpoiler:
        case Type::RightSpoiler:
            torque.setZero();
            break;
        }
    }
}

bool ActuatorEffectivenessControlSurfaces::addActuators(Configuration &configuration) {
    for (int i = 0; i < _count; i++) {
        int actuator_idx = configuration.addActuator(ActuatorType::SERVOS, _params[i].torque, Vector3f{});

        if (actuator_idx >= 0) {
            configuration.trim[configuration.selected_matrix](actuator_idx) = _params[i].trim;
        }
    }

    return true;
}

void ActuatorEffectivenessControlSurfaces::applyFlaps(float flaps_control, int first_actuator_idx, float dt,
                                                      ActuatorVector &actuator_sp) {
    _flaps_setpoint_with_slewrate.update(flaps_control, dt);

    for (int i = 0; i < _count; ++i) {
        // map [0, 1] to [-1, 1]
        // TODO: this currently only works for dedicated flaps, not flaperons
        actuator_sp(i + first_actuator_idx) += (_flaps_setpoint_with_slewrate.getState() * 2.f - 1.f) * _params[i].scale_flap;
    }
}

void ActuatorEffectivenessControlSurfaces::applySpoilers(float spoilers_control, int first_actuator_idx, float dt,
                                                         ActuatorVector &actuator_sp) {
    _spoilers_setpoint_with_slewrate.update(spoilers_control, dt);

    for (int i = 0; i < _count; ++i) {
        // TODO: this currently only works for spoilerons, not dedicated spoilers
        actuator_sp(i + first_actuator_idx) += _spoilers_setpoint_with_slewrate.getState() * _params[i].scale_spoiler;
    }
}
