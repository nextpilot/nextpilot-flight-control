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
 * @file LandDetector.h
 * Land detector interface for multicopter, fixedwing and VTOL implementations.
 *
 * @author Johan Jansen <jnsn.johan@gmail.com>
 * @author Julian Oes <julian@oes.ch>
 * @author Lorenz Meier <lorenz@px4.io>
 */

#pragma once

#include "nextpilot.h"

// #include <hysteresis/hysteresis.h>
// #include <perf/perf_counter.h>
// #include <defines.h>
// #include <module/module_command.hpp>
// #include <module/module_command.hpp>
// #include <module/module_params.hpp>
// #include <module/module_params.hpp>
// #include <px4_platform_common/px4_config.h>

// #include <workq/WorkItemScheduled.hpp>
// #include <uORB/uORBPublication.hpp>
// #include <uORB/uORBSubscription.hpp>
// #include <uORB/uORBSubscription.hpp>
// #include <uORB/topics/actuator_armed.h>
// #include <uORB/topics/parameter_update.h>
// #include <uORB/topics/sensor_selection.h>
// #include <uORB/topics/vehicle_acceleration.h>
// #include <uORB/topics/vehicle_angular_velocity.h>
// #include <uORB/topics/vehicle_imu_status.h>
// #include <uORB/topics/vehicle_land_detected.h>
// #include <uORB/topics/vehicle_local_position.h>
// #include <uORB/topics/vehicle_status.h>

using namespace time_literals;
using namespace nextpilot;
using namespace nextpilot::global_params;

namespace land_detector {

class LandDetector : public ModuleCommand<LandDetector>, ModuleParams, WorkItemScheduled {
public:
    LandDetector();
    virtual ~LandDetector();

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]) {
        return print_usage("unknown command");
    }

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    /** @see ModuleCommand::print_status() */
    int print_status() override;

    /**
     * Get the work queue going.
     */
    void start();

    static LandDetector *instantiate(int argc, char *argv[]);

protected:
    /**
     * Updates parameters.
     */
    virtual void _update_params(){};

    /**
     * Updates subscribed uORB topics.
     */
    virtual void _update_topics(){};

    /**
     * @return true if UAV is in a landed state.
     */
    virtual bool _get_landed_state() = 0;

    /**
     * @return true if UAV is in almost landed state
     */
    virtual bool _get_maybe_landed_state() {
        return false;
    }

    /**
     * @return true if UAV is touching ground but not landed
     */
    virtual bool _get_ground_contact_state() {
        return false;
    }

    /**
     * @return true if UAV is in free-fall state.
     */
    virtual bool _get_freefall_state() {
        return false;
    }

    /**
     *  @return true if vehicle could be in ground effect (close to ground)
     */
    virtual bool _get_ground_effect_state() {
        return false;
    }

    virtual bool _get_in_descend() {
        return false;
    }
    virtual bool _get_has_low_throttle() {
        return false;
    }
    virtual bool _get_horizontal_movement() {
        return false;
    }
    virtual bool _get_vertical_movement() {
        return false;
    }
    virtual bool _get_rotational_movement() {
        return false;
    }
    virtual bool _get_close_to_ground_or_skipped_check() {
        return false;
    }
    virtual void _set_hysteresis_factor(const int factor) = 0;

    Hysteresis _freefall_hysteresis{false};
    Hysteresis _landed_hysteresis{true};
    Hysteresis _maybe_landed_hysteresis{true};
    Hysteresis _ground_contact_hysteresis{true};
    Hysteresis _ground_effect_hysteresis{false};

    vehicle_local_position_s _vehicle_local_position{};
    vehicle_status_s         _vehicle_status{};

    matrix::Vector3f _acceleration{};
    matrix::Vector3f _angular_velocity{};

    bool _armed{false};
    bool _previous_armed_state{false}; ///< stores the previous actuator_armed.armed state
    bool _dist_bottom_is_observable{false};

private:
    void Run() override;

    void UpdateVehicleAtRest();

    vehicle_land_detected_s _land_detected{};
    hrt_abstime             _takeoff_time{0};
    hrt_abstime             _total_flight_time{0}; ///< total vehicle flight time in microseconds

    hrt_abstime _time_last_move_detect_us{0}; // timestamp of last movement detection event in microseconds

    perf_counter_t _cycle_perf{perf_alloc(PC_ELAPSED, MODULE_NAME ": cycle")};

    uORB::Publication<vehicle_land_detected_s> _vehicle_land_detected_pub{ORB_ID(vehicle_land_detected)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _actuator_armed_sub{ORB_ID(actuator_armed)};
    uORB::Subscription _sensor_selection_sub{ORB_ID(sensor_selection)};
    uORB::Subscription _vehicle_acceleration_sub{ORB_ID(vehicle_acceleration)};
    uORB::Subscription _vehicle_angular_velocity_sub{ORB_ID(vehicle_angular_velocity)};
    uORB::Subscription _vehicle_imu_status_sub{ORB_ID(vehicle_imu_status)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

    uORB::SubscriptionCallbackWorkItem _vehicle_local_position_sub{this, ORB_ID(vehicle_local_position)};

    uint32_t _device_id_gyro{0};

    bool _at_rest{true};

    DEFINE_PARAMETERS_CUSTOM_PARENT(
        ModuleParams,
        (ParamInt<params_id::LND_FLIGHT_T_HI>)_param_total_flight_time_high,
        (ParamInt<params_id::LND_FLIGHT_T_LO>)_param_total_flight_time_low);
};

} // namespace land_detector
