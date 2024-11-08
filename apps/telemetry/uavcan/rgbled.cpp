/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "rgbled.hpp"

UavcanRGBController::UavcanRGBController(uavcan::INode &node) :
    ModuleParams(nullptr),
    _node(node),
    _uavcan_pub_lights_cmd(node),
    _timer(node) {
    _uavcan_pub_lights_cmd.setPriority(uavcan::TransferPriority::Lowest);
}

int UavcanRGBController::init() {
    // Setup timer and call back function for periodic updates
    _timer.setCallback(TimerCbBinder(this, &UavcanRGBController::periodic_update));
    _timer.startPeriodic(uavcan::MonotonicDuration::fromMSec(1000 / MAX_RATE_HZ));
    return 0;
}

void UavcanRGBController::periodic_update(const uavcan::TimerEvent &) {
    bool                                         publish_lights = false;
    uavcan::equipment::indication::LightsCommand cmds;

    LedControlData led_control_data;

    if (_led_controller.update(led_control_data) == 1) {
        publish_lights = true;

        // RGB color in the standard 5-6-5 16-bit palette.
        // Monocolor lights should interpret this as brightness setpoint: from zero (0, 0, 0) to full brightness (31, 63, 31).
        uavcan::equipment::indication::SingleLightCommand cmd;

        uint8_t brightness = led_control_data.leds[0].brightness;

        switch (led_control_data.leds[0].color) {
        case led_control_s::COLOR_RED:
            cmd.color.red   = brightness >> 3;
            cmd.color.green = 0;
            cmd.color.blue  = 0;
            break;

        case led_control_s::COLOR_GREEN:
            cmd.color.red   = 0;
            cmd.color.green = brightness >> 2;
            cmd.color.blue  = 0;
            break;

        case led_control_s::COLOR_BLUE:
            cmd.color.red   = 0;
            cmd.color.green = 0;
            cmd.color.blue  = brightness >> 3;
            break;

        case led_control_s::COLOR_AMBER: // make it the same as yellow

        // FALLTHROUGH
        case led_control_s::COLOR_YELLOW:
            cmd.color.red   = (brightness / 2) >> 3;
            cmd.color.green = (brightness / 2) >> 2;
            cmd.color.blue  = 0;
            break;

        case led_control_s::COLOR_PURPLE:
            cmd.color.red   = (brightness / 2) >> 3;
            cmd.color.green = 0;
            cmd.color.blue  = (brightness / 2) >> 3;
            break;

        case led_control_s::COLOR_CYAN:
            cmd.color.red   = 0;
            cmd.color.green = (brightness / 2) >> 2;
            cmd.color.blue  = (brightness / 2) >> 3;
            break;

        case led_control_s::COLOR_WHITE:
            cmd.color.red   = (brightness / 3) >> 3;
            cmd.color.green = (brightness / 3) >> 2;
            cmd.color.blue  = (brightness / 3) >> 3;
            break;

        default: // led_control_s::COLOR_OFF
            cmd.color.red   = 0;
            cmd.color.green = 0;
            cmd.color.blue  = 0;
            break;
        }

        cmds.commands.push_back(cmd);
    }

    if (_armed_sub.updated()) {
        publish_lights = true;

        actuator_armed_s armed;

        if (_armed_sub.copy(&armed)) {
            /* Determine the current control mode
             *  If a light's control mode config >= current control mode, the light will be enabled
             *  Logic must match UAVCAN_LGT_* param values.
             * @value 0 Always off
             * @value 1 When autopilot is armed
             * @value 2 When autopilot is prearmed
             * @value 3 Always on
             */
            uint8_t control_mode = 0;

            if (armed.armed) {
                control_mode = 1;

            } else if (armed.prearmed) {
                control_mode = 2;

            } else {
                control_mode = 3;
            }

            uavcan::equipment::indication::SingleLightCommand cmd;

            // Beacons
            cmd.light_id = uavcan::equipment::indication::SingleLightCommand::LIGHT_ID_ANTI_COLLISION;
            cmd.color    = brightness_to_rgb565(_param_mode_anti_col.get() >= control_mode ? 255 : 0);
            cmds.commands.push_back(cmd);

            // Strobes
            cmd.light_id = uavcan::equipment::indication::SingleLightCommand::LIGHT_ID_STROBE;
            cmd.color    = brightness_to_rgb565(_param_mode_strobe.get() >= control_mode ? 255 : 0);
            cmds.commands.push_back(cmd);

            // Nav lights
            cmd.light_id = uavcan::equipment::indication::SingleLightCommand::LIGHT_ID_RIGHT_OF_WAY;
            cmd.color    = brightness_to_rgb565(_param_mode_nav.get() >= control_mode ? 255 : 0);
            cmds.commands.push_back(cmd);

            // Landing lights
            cmd.light_id = uavcan::equipment::indication::SingleLightCommand::LIGHT_ID_LANDING;
            cmd.color    = brightness_to_rgb565(_param_mode_land.get() >= control_mode ? 255 : 0);
            cmds.commands.push_back(cmd);
        }
    }

    if (publish_lights) {
        _uavcan_pub_lights_cmd.broadcast(cmds);
    }
}

uavcan::equipment::indication::RGB565 UavcanRGBController::brightness_to_rgb565(uint8_t brightness) {
    // RGB color in the standard 5-6-5 16-bit palette.
    // Monocolor lights should interpret this as brightness setpoint: from zero (0, 0, 0) to full brightness (31, 63, 31).
    uavcan::equipment::indication::RGB565 color;

    color.red   = (31.0f * (float)brightness / 255.0f);
    color.green = (62.0f * (float)brightness / 255.0f);
    color.blue  = (31.0f * (float)brightness / 255.0f);

    return color;
}
