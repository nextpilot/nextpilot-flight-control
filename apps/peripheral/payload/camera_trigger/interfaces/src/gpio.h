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
 * @file gpio.h
 *
 * Interface with cameras via FMU auxiliary pins.
 *
 */
#pragma once

#ifdef __PX4_NUTTX

#   include <board_config.h>

#   include "camera_interface.h"

class CameraInterfaceGPIO : public CameraInterface {
public:
    CameraInterfaceGPIO();
    virtual ~CameraInterfaceGPIO();

    void trigger(bool trigger_on_true);

    void info();

private:
    static const int num_gpios = 32;

    void setup();

    param_t _p_polarity{PARAM_INVALID};

    bool _trigger_invert{false};

    uint32_t _triggers[num_gpios]{};
    uint32_t _allocated_channels{0};
};

#endif /* ifdef __PX4_NUTTX */
