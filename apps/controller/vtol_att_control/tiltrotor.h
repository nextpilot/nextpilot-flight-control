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
 * @file tiltrotor.h
 *
 * @author Roman Bapst 		<bapstroman@gmail.com>
 *
 */

#ifndef TILTROTOR_H
#define TILTROTOR_H

#include "vtol_type.h"
#include <param.h>
#include <hrtimer.h>

#include <uORB/uORBPublication.hpp>
#include <uORB/topics/tiltrotor_extra_controls.h>

using namespace nextpilot::global_params;
using namespace nextpilot;

class Tiltrotor : public VtolType {
public:
    Tiltrotor(VtolAttitudeControl *_att_controller);
    ~Tiltrotor() override = default;

    void update_vtol_state() override;
    void update_transition_state() override;
    void fill_actuator_outputs() override;
    void update_mc_state() override;
    void update_fw_state() override;
    void waiting_on_tecs() override;
    void blendThrottleAfterFrontTransition(float scale) override;

private:
    enum class vtol_mode {
        MC_MODE = 0,         /**< vtol is in multicopter mode */
        TRANSITION_FRONT_P1, /**< vtol is in front transition part 1 mode */
        TRANSITION_FRONT_P2, /**< vtol is in front transition part 2 mode */
        TRANSITION_BACK,     /**< vtol is in back transition mode */
        FW_MODE              /**< vtol is in fixed wing mode */
    };

    /**
     * Specific to tiltrotor with vertical aligned rear engine/s.
     * These engines need to be shut down in fw mode. During the back-transition
     * they need to idle otherwise they need too much time to spin up for mc mode.
     */

    vtol_mode _vtol_mode{vtol_mode::MC_MODE}; /**< vtol flight mode, defined by enum vtol_mode */

    uORB::Publication<tiltrotor_extra_controls_s> _tiltrotor_extra_controls_pub{ORB_ID(tiltrotor_extra_controls)};

    float _tilt_control{0.0f}; /**< actuator value for the tilt servo */

    void  parameters_update() override;
    float timeUntilMotorsAreUp();
    float moveLinear(float start, float stop, float progress);

    void blendThrottleDuringBacktransition(const float scale, const float target_throttle);
    bool isFrontTransitionCompletedBase() override;

    hrt_abstime _last_timestamp_disarmed{0}; /**< used for calculating time since arming */
    bool        _tilt_motors_for_startup{false};

    DEFINE_PARAMETERS_CUSTOM_PARENT(VtolType,
                                    (ParamFloat<params_id::VT_TILT_MC>)_param_vt_tilt_mc,
                                    (ParamFloat<params_id::VT_TILT_TRANS>)_param_vt_tilt_trans,
                                    (ParamFloat<params_id::VT_TILT_FW>)_param_vt_tilt_fw,
                                    (ParamFloat<params_id::VT_TILT_SPINUP>)_param_vt_tilt_spinup,
                                    (ParamFloat<params_id::VT_TRANS_P2_DUR>)_param_vt_trans_p2_dur)
};
#endif
