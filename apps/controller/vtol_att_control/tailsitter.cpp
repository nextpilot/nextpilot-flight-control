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
 * @file tailsitter.cpp
 *
 * @author Roman Bapst 		<bapstroman@gmail.com>
 * @author David Vorsin     <davidvorsin@gmail.com>
 *
 */

#include "tailsitter.h"
#include "vtol_att_control_main.h"

using namespace matrix;

Tailsitter::Tailsitter(VtolAttitudeControl *attc) :
    VtolType(attc) {
}

void Tailsitter::parameters_update() {
    VtolType::updateParams();
}

void Tailsitter::update_vtol_state() {
    /* simple logic using a two way switch to perform transitions.
     * after flipping the switch the vehicle will start tilting in MC control mode, picking up
     * forward speed. After the vehicle has picked up enough and sufficient pitch angle the uav will go into FW mode.
     * For the backtransition the pitch is controlled in MC mode again and switches to full MC control reaching the sufficient pitch angle.
     */

    if (_vtol_vehicle_status->fixed_wing_system_failure) {
        // Failsafe event, switch to MC mode immediately
        _vtol_mode = vtol_mode::MC_MODE;

    } else if (!_attc->is_fixed_wing_requested()) {
        switch (_vtol_mode) { // user switchig to MC mode
        case vtol_mode::MC_MODE:
            break;

        case vtol_mode::FW_MODE:
            resetTransitionStates();
            _vtol_mode = vtol_mode::TRANSITION_BACK;
            break;

        case vtol_mode::TRANSITION_FRONT_P1:
            // failsafe into multicopter mode
            _vtol_mode = vtol_mode::MC_MODE;
            break;

        case vtol_mode::TRANSITION_BACK:
            const float pitch = Eulerf(Quatf(_v_att->q)).theta();

            float pitch_threshold_mc = PITCH_THRESHOLD_AUTO_TRANSITION_TO_MC;

            // if doing transition in Stabilized mode set threshold to max angle plus 5° margin
            if (!_v_control_mode->flag_control_altitude_enabled) {
                pitch_threshold_mc = math::radians(-_param_mpc_tilt_max.get() - 5.f);
            }

            // check if we have reached pitch angle to switch to MC mode
            if (pitch >= pitch_threshold_mc || _time_since_trans_start > _param_vt_b_trans_dur.get()) {
                _vtol_mode = vtol_mode::MC_MODE;
            }

            break;
        }

    } else { // user switchig to FW mode

        switch (_vtol_mode) {
        case vtol_mode::MC_MODE:
            // initialise a front transition
            _vtol_mode = vtol_mode::TRANSITION_FRONT_P1;
            resetTransitionStates();
            break;

        case vtol_mode::FW_MODE:
            break;

        case vtol_mode::TRANSITION_FRONT_P1: {
            if (isFrontTransitionCompleted()) {
                _vtol_mode         = vtol_mode::FW_MODE;
                _trans_finished_ts = hrt_absolute_time();
            }

            break;
        }

        case vtol_mode::TRANSITION_BACK:
            // failsafe into fixed wing mode
            _vtol_mode = vtol_mode::FW_MODE;
            break;
        }
    }

    // map tailsitter specific control phases to simple control modes
    switch (_vtol_mode) {
    case vtol_mode::MC_MODE:
        _common_vtol_mode       = mode::ROTARY_WING;
        _flag_was_in_trans_mode = false;
        break;

    case vtol_mode::FW_MODE:
        _common_vtol_mode       = mode::FIXED_WING;
        _flag_was_in_trans_mode = false;
        break;

    case vtol_mode::TRANSITION_FRONT_P1:
        _common_vtol_mode = mode::TRANSITION_TO_FW;
        break;

    case vtol_mode::TRANSITION_BACK:
        _common_vtol_mode = mode::TRANSITION_TO_MC;
        break;
    }
}

void Tailsitter::update_transition_state() {
    VtolType::update_transition_state();

    const hrt_abstime now = hrt_absolute_time();

    // we need the incoming (virtual) mc attitude setpoints to be recent, otherwise return (means the previous setpoint stays active)
    if (_mc_virtual_att_sp->timestamp < (now - 1_s)) {
        return;
    }

    if (!_flag_was_in_trans_mode) {
        _flag_was_in_trans_mode = true;

        if (_vtol_mode == vtol_mode::TRANSITION_BACK) {
            // calculate rotation axis for transition.
            _q_trans_start  = Quatf(_v_att->q);
            Vector3f z      = -_q_trans_start.dcm_z();
            _trans_rot_axis = z.cross(Vector3f(0, 0, -1));

            // as heading setpoint we choose the heading given by the direction the vehicle points
            float yaw_sp = atan2f(z(1), z(0));

            // the intial attitude setpoint for a backtransition is a combination of the current fw pitch setpoint,
            // the yaw setpoint and zero roll since we want wings level transition.
            // If for some reason the fw attitude setpoint is not recent then don't sue it and assume 0 pitch
            if (_fw_virtual_att_sp->timestamp > (now - 1_s)) {
                _q_trans_start = Eulerf(0.0f, _fw_virtual_att_sp->pitch_body, yaw_sp);

            } else {
                _q_trans_start = Eulerf(0.0f, 0.f, yaw_sp);
            }

            // attitude during transitions are controlled by mc attitude control so rotate the desired attitude to the
            // multirotor frame
            _q_trans_start = _q_trans_start * Quatf(Eulerf(0, -M_PI_2_F, 0));

        } else if (_vtol_mode == vtol_mode::TRANSITION_FRONT_P1) {
            // initial attitude setpoint for the transition should be with wings level
            _q_trans_start  = Eulerf(0.0f, _mc_virtual_att_sp->pitch_body, _mc_virtual_att_sp->yaw_body);
            Vector3f x      = Dcmf(Quatf(_v_att->q)) * Vector3f(1, 0, 0);
            _trans_rot_axis = -x.cross(Vector3f(0, 0, -1));
        }

        _q_trans_sp = _q_trans_start;
    }

    // ensure input quaternions are exactly normalized because acosf(1.00001) == NaN
    _q_trans_sp.normalize();

    // tilt angle (zero if vehicle nose points up (hover))
    float cos_tilt   = _q_trans_sp(0) * _q_trans_sp(0) - _q_trans_sp(1) * _q_trans_sp(1) - _q_trans_sp(2) * _q_trans_sp(2) + _q_trans_sp(3) * _q_trans_sp(3);
    cos_tilt         = cos_tilt > 1.0f ? 1.0f : cos_tilt;
    cos_tilt         = cos_tilt < -1.0f ? -1.0f : cos_tilt;
    const float tilt = acosf(cos_tilt);

    if (_vtol_mode == vtol_mode::TRANSITION_FRONT_P1) {
        // calculate pitching rate - and constrain to at least 0.1s transition time
        const float trans_pitch_rate = M_PI_2_F / math::max(_param_vt_f_trans_dur.get(), 0.1f);

        if (tilt < M_PI_2_F - math::radians(_param_fw_psp_off.get())) {
            _q_trans_sp = Quatf(AxisAnglef(_trans_rot_axis,
                                           _time_since_trans_start * trans_pitch_rate)) *
                          _q_trans_start;
        }

    } else if (_vtol_mode == vtol_mode::TRANSITION_BACK) {
        // calculate pitching rate - and constrain to at least 0.1s transition time
        const float trans_pitch_rate = M_PI_2_F / math::max(_param_vt_b_trans_dur.get(), 0.1f);

        if (tilt > 0.01f) {
            _q_trans_sp = Quatf(AxisAnglef(_trans_rot_axis,
                                           _time_since_trans_start * trans_pitch_rate)) *
                          _q_trans_start;
        }
    }

    _v_att_sp->thrust_body[2] = _mc_virtual_att_sp->thrust_body[2];

    _v_att_sp->timestamp = hrt_absolute_time();

    const Eulerf euler_sp(_q_trans_sp);
    _v_att_sp->roll_body  = euler_sp.phi();
    _v_att_sp->pitch_body = euler_sp.theta();
    _v_att_sp->yaw_body   = euler_sp.psi();

    _q_trans_sp.copyTo(_v_att_sp->q_d);
}

void Tailsitter::waiting_on_tecs() {
    // copy the last trust value from the front transition
    _v_att_sp->thrust_body[0] = _thrust_transition;
}

void Tailsitter::update_fw_state() {
    VtolType::update_fw_state();
}

/**
 * Write data to actuator output topic.
 */
void Tailsitter::fill_actuator_outputs() {
    _torque_setpoint_0->timestamp        = hrt_absolute_time();
    _torque_setpoint_0->timestamp_sample = _vehicle_torque_setpoint_virtual_mc->timestamp_sample;
    _torque_setpoint_0->xyz[0]           = 0.f;
    _torque_setpoint_0->xyz[1]           = 0.f;
    _torque_setpoint_0->xyz[2]           = 0.f;

    _torque_setpoint_1->timestamp        = hrt_absolute_time();
    _torque_setpoint_1->timestamp_sample = _vehicle_torque_setpoint_virtual_fw->timestamp_sample;
    _torque_setpoint_1->xyz[0]           = 0.f;
    _torque_setpoint_1->xyz[1]           = 0.f;
    _torque_setpoint_1->xyz[2]           = 0.f;

    _thrust_setpoint_0->timestamp        = hrt_absolute_time();
    _thrust_setpoint_0->timestamp_sample = _vehicle_thrust_setpoint_virtual_mc->timestamp_sample;
    _thrust_setpoint_0->xyz[0]           = 0.f;
    _thrust_setpoint_0->xyz[1]           = 0.f;
    _thrust_setpoint_0->xyz[2]           = 0.f;

    _thrust_setpoint_1->timestamp        = hrt_absolute_time();
    _thrust_setpoint_1->timestamp_sample = _vehicle_thrust_setpoint_virtual_fw->timestamp_sample;
    _thrust_setpoint_1->xyz[0]           = 0.f;
    _thrust_setpoint_1->xyz[1]           = 0.f;
    _thrust_setpoint_1->xyz[2]           = 0.f;

    // Motors
    if (_vtol_mode == vtol_mode::FW_MODE) {
        _thrust_setpoint_0->xyz[2] = -_vehicle_thrust_setpoint_virtual_fw->xyz[0];

        /* allow differential thrust if enabled */
        if (_param_vt_fw_difthr_en.get() & static_cast<int32_t>(VtFwDifthrEnBits::YAW_BIT)) {
            _torque_setpoint_0->xyz[0] = _vehicle_torque_setpoint_virtual_fw->xyz[0] * _param_vt_fw_difthr_s_y.get();
        }

        if (_param_vt_fw_difthr_en.get() & static_cast<int32_t>(VtFwDifthrEnBits::PITCH_BIT)) {
            _torque_setpoint_0->xyz[1] = _vehicle_torque_setpoint_virtual_fw->xyz[1] * _param_vt_fw_difthr_s_p.get();
        }

        if (_param_vt_fw_difthr_en.get() & static_cast<int32_t>(VtFwDifthrEnBits::ROLL_BIT)) {
            _torque_setpoint_0->xyz[2] = _vehicle_torque_setpoint_virtual_fw->xyz[2] * _param_vt_fw_difthr_s_r.get();
        }

    } else {
        _torque_setpoint_0->xyz[0] = _vehicle_torque_setpoint_virtual_mc->xyz[0];
        _torque_setpoint_0->xyz[1] = _vehicle_torque_setpoint_virtual_mc->xyz[1];
        _torque_setpoint_0->xyz[2] = _vehicle_torque_setpoint_virtual_mc->xyz[2];

        _thrust_setpoint_0->xyz[2] = _vehicle_thrust_setpoint_virtual_mc->xyz[2];
    }

    // Control surfaces
    if (!_param_vt_elev_mc_lock.get() || _vtol_mode != vtol_mode::MC_MODE) {
        _torque_setpoint_1->xyz[0] = _vehicle_torque_setpoint_virtual_fw->xyz[0];
        _torque_setpoint_1->xyz[1] = _vehicle_torque_setpoint_virtual_fw->xyz[1];
        _torque_setpoint_1->xyz[2] = _vehicle_torque_setpoint_virtual_fw->xyz[2];
    }
}

bool Tailsitter::isFrontTransitionCompletedBase() {
    const bool airspeed_triggers_transition = PX4_ISFINITE(_airspeed_validated->calibrated_airspeed_m_s) && !_param_fw_arsp_mode.get();

    bool        transition_to_fw = false;
    const float pitch            = Eulerf(Quatf(_v_att->q)).theta();

    float pitch_threshold_fw = PITCH_THRESHOLD_AUTO_TRANSITION_TO_FW;

    // if doing transition in Stabilized mode set threshold to max angle minus 5° margin
    if (!_v_control_mode->flag_control_altitude_enabled) {
        pitch_threshold_fw = math::radians(-_param_mpc_tilt_max.get() + 5.f);
    }

    if (pitch <= pitch_threshold_fw) {
        if (airspeed_triggers_transition) {
            transition_to_fw = _airspeed_validated->calibrated_airspeed_m_s >= _param_vt_arsp_trans.get();

        } else {
            transition_to_fw = true;
        }
    }

    return transition_to_fw;
}
