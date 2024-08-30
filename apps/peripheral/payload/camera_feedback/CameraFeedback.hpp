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
 *
 * Online and offline geotagging from camera feedback
 *
 * @author Mohammed Kabir <kabir@uasys.io>
 */

#pragma once

#include <mathlib/mathlib.h>
#include <parameters/param.h>

#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>

#include <workq/WorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/camera_capture.h>
#include <uORB/topics/camera_trigger.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/gimbal_device_attitude_status.h>

class CameraFeedback : public ModuleCommand<CameraFeedback>, public ModuleParams, public nextpilot::WorkItem {
public:
    CameraFeedback();
    ~CameraFeedback() override = default;

    /** @see ModuleCommand */
    static int task_spawn(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    bool init();

private:
    void Run() override;

    uORB::SubscriptionCallbackWorkItem _trigger_sub{this, ORB_ID(camera_trigger)};

    uORB::Subscription _gpos_sub{ORB_ID(vehicle_global_position)};
    uORB::Subscription _att_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription _gimbal_sub{ORB_ID(gimbal_device_attitude_status)};

    uORB::Publication<camera_capture_s> _capture_pub{ORB_ID(camera_capture)};

    param_t _p_cam_cap_fback;
    int32_t _cam_cap_fback{0};
};
