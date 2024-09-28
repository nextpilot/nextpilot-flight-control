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
 * @file tailsitter.h
 *
 * @author Roman Bapst 		<bapstroman@gmail.com>
 * @author David Vorsin     <davidvorsin@gmail.com>
 *
 */

#ifndef TAILSITTER_H
#define TAILSITTER_H

#include "vtol_type.h"

#include <param.h>
#include <hrtimer.h>
#include <matrix/matrix/math.hpp>

using namespace nextpilot::param;

// [rad] Pitch threshold required for completing transition to fixed-wing in automatic transitions
static constexpr float PITCH_THRESHOLD_AUTO_TRANSITION_TO_FW = -1.05f; // -60°

// [rad] Pitch threshold required for completing transition to hover in automatic transitions
static constexpr float PITCH_THRESHOLD_AUTO_TRANSITION_TO_MC = -0.26f; // -15°

class Tailsitter : public VtolType {
public:
    Tailsitter(VtolAttitudeControl *_att_controller);
    ~Tailsitter() override = default;

    void update_vtol_state() override;
    void update_transition_state() override;
    void update_fw_state() override;
    void fill_actuator_outputs() override;
    void waiting_on_tecs() override;

private:
    enum class vtol_mode {
        MC_MODE = 0,         /**< vtol is in multicopter mode */
        TRANSITION_FRONT_P1, /**< vtol is in front transition part 1 mode */
        TRANSITION_BACK,     /**< vtol is in back transition mode */
        FW_MODE              /**< vtol is in fixed wing mode */
    };

    vtol_mode _vtol_mode{vtol_mode::MC_MODE}; /**< vtol flight mode, defined by enum vtol_mode */

    bool _flag_was_in_trans_mode = false;     // true if mode has just switched to transition

    matrix::Quatf    _q_trans_start;
    matrix::Quatf    _q_trans_sp;
    matrix::Vector3f _trans_rot_axis;

    void parameters_update() override;

    bool isFrontTransitionCompletedBase() override;

    DEFINE_PARAMETERS_CUSTOM_PARENT(VtolType,
                                    (ParamFloat<params_id::FW_PSP_OFF>)_param_fw_psp_off,
                                    (ParamFloat<params_id::MPC_MAN_TILT_MAX>)_param_mpc_tilt_max)
};
#endif
