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
 * @file VTOL_att_control_main.cpp
 * Implementation of an attitude controller for VTOL airframes. This module receives data
 * from both the fixed wing- and the multicopter attitude controllers and processes it.
 * It computes the correct actuator controls depending on which mode the vehicle is in (hover,forward-
 * flight or transition). It also publishes the resulting controls on the actuator controls topics.
 *
 * @author Roman Bapst 		<bapstr@ethz.ch>
 * @author Lorenz Meier 	<lm@inf.ethz.ch>
 * @author Thomas Gubler	<thomasgubler@gmail.com>
 * @author David Vorsin		<davidvorsin@gmail.com>
 * @author Sander Smeets	<sander@droneslab.com>
 * @author Andreas Antener 	<andreas@uaventure.com>
 *
 */

#pragma once

#include <hrtimer.h>
#include <geo/geo.h>
#include <mathlib/mathlib.h>
#include <perf/perf_counter.h>
#include <matrix/math.hpp>
#include <defines.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItem.hpp>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/action_request.h>
#include <uORB/topics/airspeed_validated.h>
#include <uORB/topics/home_position.h>
#include <uORB/topics/normalized_unsigned_setpoint.h>
#include <uORB/topics/vehicle_air_data.h>
#include <uORB/topics/parameter_update.h>
#include <uORB/topics/position_setpoint_triplet.h>
#include <uORB/topics/tecs_status.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_land_detected.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/vehicle_local_position_setpoint.h>
#include <uORB/topics/vtol_vehicle_status.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/vehicle_thrust_setpoint.h>
#include <uORB/topics/vehicle_torque_setpoint.h>

#include "standard.h"
#include "tailsitter.h"
#include "tiltrotor.h"

using namespace time_literals;
using namespace nextpilot::uORB;
using namespace nextpilot::workq;
using namespace nextpilot::param;

extern "C" __EXPORT int vtol_att_control_main(int argc, char *argv[]);

static constexpr float kMaxVTOLAttitudeControlTimeStep = 0.1f; // max time step [s]

class VtolAttitudeControl : public ModuleCommand<VtolAttitudeControl>, public ModuleParams, public WorkItem {
public:
    VtolAttitudeControl();
    ~VtolAttitudeControl() override;

    /** @see ModuleCommand */
    static VtolAttitudeControl *instantiate(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

    bool is_fixed_wing_requested() {
        return _transition_command == vtol_vehicle_status_s::VEHICLE_VTOL_STATE_FW;
    }

    void quadchute(QuadchuteReason reason);

    int get_transition_command() {
        return _transition_command;
    }

    bool get_immediate_transition() {
        return _immediate_transition;
    }

    void reset_immediate_transition() {
        _immediate_transition = false;
    }

    float getAirDensity() const {
        return _air_density;
    }

    struct vehicle_torque_setpoint_s *get_vehicle_torque_setpoint_virtual_mc() {
        return &_vehicle_torque_setpoint_virtual_mc;
    }

    struct vehicle_torque_setpoint_s *get_vehicle_torque_setpoint_virtual_fw() {
        return &_vehicle_torque_setpoint_virtual_fw;
    }

    struct vehicle_thrust_setpoint_s *get_vehicle_thrust_setpoint_virtual_mc() {
        return &_vehicle_thrust_setpoint_virtual_mc;
    }

    struct vehicle_thrust_setpoint_s *get_vehicle_thrust_setpoint_virtual_fw() {
        return &_vehicle_thrust_setpoint_virtual_fw;
    }

    struct airspeed_validated_s *get_airspeed() {
        return &_airspeed_validated;
    }

    struct position_setpoint_triplet_s *get_pos_sp_triplet() {
        return &_pos_sp_triplet;
    }

    struct tecs_status_s *get_tecs_status() {
        return &_tecs_status;
    }

    struct vehicle_attitude_s *get_att() {
        return &_vehicle_attitude;
    }

    struct vehicle_attitude_setpoint_s *get_att_sp() {
        return &_vehicle_attitude_sp;
    }

    struct vehicle_attitude_setpoint_s *get_fw_virtual_att_sp() {
        return &_fw_virtual_att_sp;
    }

    struct vehicle_attitude_setpoint_s *get_mc_virtual_att_sp() {
        return &_mc_virtual_att_sp;
    }

    struct vehicle_control_mode_s *get_control_mode() {
        return &_vehicle_control_mode;
    }

    struct vehicle_land_detected_s *get_land_detected() {
        return &_land_detected;
    }

    struct vehicle_local_position_s *get_local_pos() {
        return &_local_pos;
    }

    struct vehicle_local_position_setpoint_s *get_local_pos_sp() {
        return &_local_pos_sp;
    }

    struct vehicle_torque_setpoint_s *get_torque_setpoint_0() {
        return &_torque_setpoint_0;
    }

    struct vehicle_torque_setpoint_s *get_torque_setpoint_1() {
        return &_torque_setpoint_1;
    }

    struct vehicle_thrust_setpoint_s *get_thrust_setpoint_0() {
        return &_thrust_setpoint_0;
    }

    struct vehicle_thrust_setpoint_s *get_thrust_setpoint_1() {
        return &_thrust_setpoint_1;
    }

    struct vtol_vehicle_status_s *get_vtol_vehicle_status() {
        return &_vtol_vehicle_status;
    }

    float get_home_position_z() {
        return _home_position_z;
    }

private:
    void                               Run() override;
    uORB::SubscriptionCallbackWorkItem _vehicle_torque_setpoint_virtual_fw_sub{this, ORB_ID(vehicle_torque_setpoint_virtual_fw)};
    uORB::SubscriptionCallbackWorkItem _vehicle_torque_setpoint_virtual_mc_sub{this, ORB_ID(vehicle_torque_setpoint_virtual_mc)};
    uORB::SubscriptionCallbackWorkItem _vehicle_thrust_setpoint_virtual_fw_sub{this, ORB_ID(vehicle_thrust_setpoint_virtual_fw)};
    uORB::SubscriptionCallbackWorkItem _vehicle_thrust_setpoint_virtual_mc_sub{this, ORB_ID(vehicle_thrust_setpoint_virtual_mc)};

    uORB::SubscriptionInterval _parameter_update_sub{ORB_ID(parameter_update), 1_s};

    uORB::Subscription _action_request_sub{ORB_ID(action_request)};
    uORB::Subscription _airspeed_validated_sub{ORB_ID(airspeed_validated)};
    uORB::Subscription _fw_virtual_att_sp_sub{ORB_ID(fw_virtual_attitude_setpoint)};
    uORB::Subscription _home_position_sub{ORB_ID(home_position)};
    uORB::Subscription _land_detected_sub{ORB_ID(vehicle_land_detected)};
    uORB::Subscription _local_pos_sp_sub{ORB_ID(vehicle_local_position_setpoint)};
    uORB::Subscription _local_pos_sub{ORB_ID(vehicle_local_position)};
    uORB::Subscription _mc_virtual_att_sp_sub{ORB_ID(mc_virtual_attitude_setpoint)};
    uORB::Subscription _pos_sp_triplet_sub{ORB_ID(position_setpoint_triplet)};
    uORB::Subscription _tecs_status_sub{ORB_ID(tecs_status)};
    uORB::Subscription _vehicle_air_data_sub{ORB_ID(vehicle_air_data)};
    uORB::Subscription _vehicle_attitude_sub{ORB_ID(vehicle_attitude)};
    uORB::Subscription _vehicle_control_mode_sub{ORB_ID(vehicle_control_mode)};
    uORB::Subscription _vehicle_cmd_sub{ORB_ID(vehicle_command)};
    uORB::Subscription _vehicle_status_sub{ORB_ID(vehicle_status)};

    uORB::Publication<normalized_unsigned_setpoint_s> _flaps_setpoint_pub{ORB_ID(flaps_setpoint)};
    uORB::Publication<normalized_unsigned_setpoint_s> _spoilers_setpoint_pub{ORB_ID(spoilers_setpoint)};
    uORB::Publication<vehicle_attitude_setpoint_s>    _vehicle_attitude_sp_pub{ORB_ID(vehicle_attitude_setpoint)};
    uORB::PublicationMulti<vehicle_thrust_setpoint_s> _vehicle_thrust_setpoint0_pub{ORB_ID(vehicle_thrust_setpoint)};
    uORB::PublicationMulti<vehicle_thrust_setpoint_s> _vehicle_thrust_setpoint1_pub{ORB_ID(vehicle_thrust_setpoint)};
    uORB::PublicationMulti<vehicle_torque_setpoint_s> _vehicle_torque_setpoint0_pub{ORB_ID(vehicle_torque_setpoint)};
    uORB::PublicationMulti<vehicle_torque_setpoint_s> _vehicle_torque_setpoint1_pub{ORB_ID(vehicle_torque_setpoint)};
    uORB::Publication<vtol_vehicle_status_s>          _vtol_vehicle_status_pub{ORB_ID(vtol_vehicle_status)};

    orb_advert_t _mavlink_log_pub{nullptr};             // mavlink log uORB handle

    vehicle_attitude_setpoint_s _vehicle_attitude_sp{}; // vehicle attitude setpoint
    vehicle_attitude_setpoint_s _fw_virtual_att_sp{};   // virtual fw attitude setpoint
    vehicle_attitude_setpoint_s _mc_virtual_att_sp{};   // virtual mc attitude setpoint

    vehicle_torque_setpoint_s _vehicle_torque_setpoint_virtual_mc{};
    vehicle_torque_setpoint_s _vehicle_torque_setpoint_virtual_fw{};
    vehicle_thrust_setpoint_s _vehicle_thrust_setpoint_virtual_mc{};
    vehicle_thrust_setpoint_s _vehicle_thrust_setpoint_virtual_fw{};

    vehicle_torque_setpoint_s _torque_setpoint_0{};
    vehicle_torque_setpoint_s _torque_setpoint_1{};
    vehicle_thrust_setpoint_s _thrust_setpoint_0{};
    vehicle_thrust_setpoint_s _thrust_setpoint_1{};

    airspeed_validated_s              _airspeed_validated{};
    position_setpoint_triplet_s       _pos_sp_triplet{};
    tecs_status_s                     _tecs_status{};
    vehicle_attitude_s                _vehicle_attitude{};
    vehicle_control_mode_s            _vehicle_control_mode{};
    vehicle_land_detected_s           _land_detected{};
    vehicle_local_position_s          _local_pos{};
    vehicle_local_position_setpoint_s _local_pos_sp{};
    vehicle_status_s                  _vehicle_status{};
    vtol_vehicle_status_s             _vtol_vehicle_status{};
    float                             _home_position_z{NAN};

    float _air_density{CONSTANTS_AIR_DENSITY_SEA_LEVEL_15C}; // [kg/m^3]

    hrt_abstime _last_run_timestamp{0};

    /* For multicopters it is usual to have a non-zero idle speed of the engines
     * for fixed wings we want to have an idle speed of zero since we do not want
     * to waste energy when gliding. */
    int  _transition_command{vtol_vehicle_status_s::VEHICLE_VTOL_STATE_MC};
    bool _immediate_transition{false};

    uint8_t _nav_state_prev;

    VtolType *_vtol_type{nullptr}; // base class for different vtol types

    bool _initialized{false};

    perf_counter_t _loop_perf; // loop performance counter

    void vehicle_status_poll();

    void action_request_poll();

    void vehicle_cmd_poll();

    void parameters_update();

    DEFINE_PARAMETERS(
        (ParamInt<params_id::VT_TYPE>)_param_vt_type,
        (ParamFloat<params_id::VT_SPOILER_MC_LD>)_param_vt_spoiler_mc_ld)
};
