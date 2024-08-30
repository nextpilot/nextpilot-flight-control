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

#   include "gpio.h"

#   include <cstring>
#   include <px4_arch/io_timer.h>

CameraInterfaceGPIO::CameraInterfaceGPIO() {
    _p_polarity = param_find("TRIG_POLARITY");

    // polarity of the trigger (0 = active low, 1 = active high )
    int32_t polarity = 0;
    param_get(_p_polarity, &polarity);
    _trigger_invert = (polarity == 0);

    get_pins();
    setup();
}

CameraInterfaceGPIO::~CameraInterfaceGPIO() {
    unsigned channel = 0;

    while (_allocated_channels != 0) {
        if (((1u << channel) & _allocated_channels)) {
            io_timer_unallocate_channel(channel);
            _allocated_channels &= ~(1u << channel);
        }

        ++channel;
    }
}

void CameraInterfaceGPIO::setup() {
    _allocated_channels = 0;

    for (unsigned i = 0, t = 0; i < arraySize(_pins); i++) {
        // Pin range is from 0 to num_gpios - 1
        if (_pins[i] >= 0 && t < (int)arraySize(_triggers)) {
            uint32_t gpio = io_timer_channel_get_gpio_output(_pins[i]);

            if (io_timer_allocate_channel(_pins[i], IOTimerChanMode_Trigger) == 0) {
                _allocated_channels |= 1 << _pins[i];
                _triggers[t++]       = gpio;
                px4_arch_configgpio(gpio);
                px4_arch_gpiowrite(gpio, false ^ _trigger_invert);
            }
        }
    }
}

void CameraInterfaceGPIO::trigger(bool trigger_on_true) {
    bool trigger_state = trigger_on_true ^ _trigger_invert;

    for (unsigned i = 0; i < arraySize(_triggers); i++) {
        if (_triggers[i] != 0) {
            px4_arch_gpiowrite(_triggers[i], trigger_state);
        }
    }
}

void CameraInterfaceGPIO::info() {
    LOG_RAW("GPIO trigger mode, pins enabled: ");

    for (unsigned i = 0; i < arraySize(_pins); ++i) {
        if (_pins[i] < 0) {
            continue;
        }

        LOG_RAW("[%d]", _pins[i] + 1);
    }

    LOG_RAW(", polarity : %s\n", _trigger_invert ? "ACTIVE_LOW" : "ACTIVE_HIGH");
}

#endif /* ifdef __PX4_NUTTX */
