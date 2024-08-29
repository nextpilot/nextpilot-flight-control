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
 * @file pwm.h
 *
 * Interface with cameras via pwm.
 *
 */
#pragma once

#ifdef __PX4_NUTTX

#   include <drivers/drv_hrt.h>
#   include <parameters/param.h>
#   include <px4_platform_common/log.h>

#   include "camera_interface.h"

class CameraInterfacePWM : public CameraInterface {
public:
    CameraInterfacePWM();
    virtual ~CameraInterfacePWM();

    void trigger(bool trigger_on_true);

    void info();

private:
    void setup();

    int32_t _pwm_camera_shoot{0};
    int32_t _pwm_camera_neutral{0};
};

#endif /* ifdef __PX4_NUTTX */
