/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifdef __PX4_NUTTX

#   include <sys/ioctl.h>
#   include <lib/mathlib/mathlib.h>
#   include <parameters/param.h>


#   include "drivers/drv_pwm_trigger.h"
#   include "pwm.h"

CameraInterfacePWM::CameraInterfacePWM() :
    CameraInterface() {
    param_get(param_find("TRIG_PWM_SHOOT"), &_pwm_camera_shoot);
    param_get(param_find("TRIG_PWM_NEUTRAL"), &_pwm_camera_neutral);
    get_pins();
    setup();
}

CameraInterfacePWM::~CameraInterfacePWM() {
    // Deinitialise trigger channels
    up_pwm_trigger_deinit();
}

void CameraInterfacePWM::setup() {
    // Precompute the bitmask for enabled channels
    uint32_t pin_bitmask = 0;

    for (unsigned i = 0; i < arraySize(_pins); i++) {
        if (_pins[i] >= 0) {
            pin_bitmask |= (1 << _pins[i]);
        }
    }

    // Initialize and arm channels
    int ret = up_pwm_trigger_init(pin_bitmask);

    if (ret < 0) {
        PX4_ERR("up_pwm_trigger_init failed (%i)", ret);
        pin_bitmask = 0;

    } else {
        pin_bitmask = ret;
    }

    // Clear pins that could not be initialized
    for (unsigned i = 0; i < arraySize(_pins); i++) {
        if (_pins[i] >= 0 && ((1 << _pins[i]) & pin_bitmask) == 0) {
            _pins[i] = -1;
        }
    }

    // Set neutral pulsewidths
    for (unsigned i = 0; i < arraySize(_pins); i++) {
        if (_pins[i] >= 0) {
            up_pwm_trigger_set(_pins[i], math::constrain(_pwm_camera_neutral, (int32_t)0, (int32_t)2000));
        }
    }
}

void CameraInterfacePWM::trigger(bool trigger_on_true) {
    for (unsigned i = 0; i < arraySize(_pins); i++) {
        if (_pins[i] >= 0) {
            // Set all valid pins to shoot or neutral levels
            up_pwm_trigger_set(_pins[i], math::constrain(trigger_on_true ? _pwm_camera_shoot : _pwm_camera_neutral, (int32_t)0,
                                                         (int32_t)2000));
        }
    }
}

void CameraInterfacePWM::info() {
    PX4_INFO_RAW("PWM trigger mode, pins enabled: ");

    for (unsigned i = 0; i < arraySize(_pins); ++i) {
        if (_pins[i] < 0) {
            continue;
        }

        PX4_INFO_RAW("[%d]", _pins[i] + 1);
    }

    PX4_INFO_RAW("\n");
}

#endif /* ifdef __PX4_NUTTX */
