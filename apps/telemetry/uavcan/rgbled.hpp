/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <uORB/topics/actuator_armed.h>

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/indication/LightsCommand.hpp>

#include <led/led.h>
#include <module/module_params.hpp>

class UavcanRGBController : public ModuleParams {
public:
    UavcanRGBController(uavcan::INode &node);
    ~UavcanRGBController() = default;

    // setup periodic updater
    int init();

private:
    // Max update rate to avoid exessive bus traffic
    static constexpr unsigned MAX_RATE_HZ = 20;

    void periodic_update(const uavcan::TimerEvent &);

    uavcan::equipment::indication::RGB565 brightness_to_rgb565(uint8_t brightness);

    typedef uavcan::MethodBinder<UavcanRGBController *, void (UavcanRGBController::*)(const uavcan::TimerEvent &)>
        TimerCbBinder;

    uavcan::INode                                                  &_node;
    uavcan::Publisher<uavcan::equipment::indication::LightsCommand> _uavcan_pub_lights_cmd;
    uavcan::TimerEventForwarder<TimerCbBinder>                      _timer;

    uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};

    LedController _led_controller;

    DEFINE_PARAMETERS(
        (ParamInt<px4::params::UAVCAN_LGT_ANTCL>)_param_mode_anti_col,
        (ParamInt<px4::params::UAVCAN_LGT_STROB>)_param_mode_strobe,
        (ParamInt<px4::params::UAVCAN_LGT_NAV>)_param_mode_nav,
        (ParamInt<px4::params::UAVCAN_LGT_LAND>)_param_mode_land)
};
