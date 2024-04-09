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
 * @file LaunchDetector.h
 * Auto launch detection for catapult/hand-launch vehicles
 *
 * @author Thomas Gubler <thomasgubler@gmail.com>
 */

#ifndef LAUNCHDETECTOR_H
#define LAUNCHDETECTOR_H

#include <px4_platform_common/module_params.hpp>
#include <uORB/topics/launch_detection_status.h>

namespace launchdetection {

// Info delay threshold (to publish info every kInfoDelay seconds)
static constexpr float kInfoDelay = 4.f;

class __EXPORT LaunchDetector : public ModuleParams {
public:
    LaunchDetector(ModuleParams *parent) :
        ModuleParams(parent) {
    }
    ~LaunchDetector() = default;

    LaunchDetector(const LaunchDetector &)           = delete;
    LaunchDetector operator=(const LaunchDetector &) = delete;

    /**
     * @brief Reset launch detection state machine.
     */
    void reset();

    /**
     * @brief Updates the state machine based on the current vehicle condition.
     *
     * @param dt Time step [us]
     * @param accel_x Measured acceleration in body x [m/s/s]
     */
    void update(const float dt, const float accel_x);

    /**
     * @brief Get the Launch Detected state
     *
     * @return uint (aligned with launch_detection_status_s::launch_detection_state)
     */
    uint getLaunchDetected() const;

    /**
     * @brief Forces state of launch detection state machine to STATE_FLYING.
     */
    void forceSetFlyState() {
        state_ = launch_detection_status_s::STATE_FLYING;
    }

private:
    /**
     * Motor delay counter [s]
     */
    float motor_delay_counter_{0.f};

    /**
     * Info delay counter (to publish info every kInfoDelay seconds) [s]
     */
    float info_delay_counter_s_{kInfoDelay};

    /**
     *  Counter for how long the measured acceleration is above the defined threshold [s]
     */
    float acceleration_detected_counter_{0.f};

    /**
     * Current state of the launch detection state machine [launch_detection_status_s::launch_detection_state]
     */
    uint state_{launch_detection_status_s::STATE_WAITING_FOR_LAUNCH};

    DEFINE_PARAMETERS(
        (ParamFloat<params_id::FW_LAUN_AC_THLD>)param_fw_laun_ac_thld_,
        (ParamFloat<params_id::FW_LAUN_AC_T>)param_fw_laun_ac_t_,
        (ParamFloat<params_id::FW_LAUN_MOT_DEL>)param_fw_laun_mot_del_)
};

} // namespace launchdetection

#endif // LAUNCHDETECTOR_H
