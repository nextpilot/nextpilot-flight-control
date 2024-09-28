/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "safety"
#define LOG_LVL LOG_LVL_INFO

#include <rtdbg.h>
#include "SafetyButton.hpp"

#ifndef GPIO_SAFETY_SWITCH_IN
#   error "board needs to define a safety button gpio pin to use this module"
#endif

using namespace time_literals;

static constexpr uint8_t CYCLE_COUNT{30}; /* safety switch must be held for 1 second to activate */

// Define the various LED flash sequences for each system state.
enum class LED_PATTERN : uint16_t {
    FMU_OK_TO_ARM     = 0x0003, /**< slow blinking */
    FMU_REFUSE_TO_ARM = 0x5555, /**< fast blinking */
    IO_ARMED          = 0x5050, /**< long off, then double blink */
    FMU_ARMED         = 0x5500, /**< long off, then quad blink */
    IO_FMU_ARMED      = 0xffff, /**< constantly on */
};

SafetyButton::SafetyButton() :
    WorkItemScheduled(MODULE_NAME, wq_configurations::hp_default) {
    _has_px4io = false;
    //PX4_MFT_HW_SUPPORTED(PX4_MFT_PX4IO);
}

SafetyButton::~SafetyButton() {
    ScheduleClear();
}

void SafetyButton::CheckSafetyRequest(bool button_pressed) {
    /* Keep button pressed for one second to turn off safety */
    if (button_pressed) {
        if (_button_counter <= CYCLE_COUNT) {
            _button_counter++;
        }

        if (_button_counter == CYCLE_COUNT) {
            _button_publisher.safetyButtonTriggerEvent();
        }

    } else {
        _button_counter = 0;
    }
}

void SafetyButton::CheckPairingRequest(bool button_pressed) {
    // Need to press the button 3 times within 2 seconds
    const hrt_abstime now = hrt_absolute_time();

    if (now - _pairing_start > 2_s) {
        // reset state
        _pairing_start          = 0;
        _pairing_button_counter = 0;
    }

    if (!_button_prev_sate && button_pressed) {
        if (_pairing_start == 0) {
            _pairing_start = now;
        }

        ++_pairing_button_counter;
    }

    if (_pairing_button_counter == ButtonPublisher::PAIRING_BUTTON_EVENT_COUNT) {
        _button_publisher.pairingButtonTriggerEvent();
        // reset state
        _pairing_start          = 0;
        _pairing_button_counter = 0;
    }
}

void SafetyButton::FlashButton() {
#if defined(GPIO_SAFETY_SWITCH_LED)
    actuator_armed_s armed;

    if (_armed_sub.copy(&armed)) {
        // Select the appropriate LED flash pattern depending on the current arm state
        LED_PATTERN pattern = LED_PATTERN::FMU_REFUSE_TO_ARM;

        // cycle the blink state machine
        if (_button_prev_sate) {
            if (armed.armed) {
                pattern = LED_PATTERN::IO_FMU_ARMED;

            } else {
                pattern = LED_PATTERN::IO_ARMED;
            }

        } else if (armed.armed) {
            pattern = LED_PATTERN::FMU_ARMED;

        } else {
            pattern = LED_PATTERN::FMU_OK_TO_ARM;
        }

        // Turn the LED on if we have a 1 at the current bit position
        rt_pin_write(GPIO_SAFETY_SWITCH_LED, !((uint16_t)pattern & (1 << (_blink_counter++ / 3))));

        if (_blink_counter > 45) {
            _blink_counter = 0;
        }
    }

#endif // GPIO_SAFETY_SWITCH_LED
}

void SafetyButton::Run() {
    if (should_exit()) {
        exit_and_cleanup();
        return;
    }

    const bool button_pressed = rt_pin_read(GPIO_SAFETY_SWITCH_IN);

    // control safety switch LED & safety button
    if (!_has_px4io) {
        FlashButton();
        CheckSafetyRequest(button_pressed);
    }

    CheckPairingRequest(button_pressed);
    _button_prev_sate = button_pressed;
}

int SafetyButton::init() {
    rt_pin_mode(GPIO_SAFETY_SWITCH_IN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(GPIO_SAFETY_SWITCH_LED, PIN_MODE_OUTPUT);

    ScheduleOnInterval(33_ms); // run at 30 Hz

    return RT_EOK;
}

int SafetyButton::custom_command(int argc, char *argv[]) {
    return print_usage("unknown command");
}

int SafetyButton::print_usage(const char *reason) {
    if (reason) {
        LOG_RAW("%s\n", reason);
    }

    PRINT_MODULE_DESCRIPTION(
        R"DESCR_STR(
### Description
This module is responsible for the safety button.
Pressing the safety button 3 times quickly will trigger a GCS pairing request.

)DESCR_STR");

    PRINT_MODULE_USAGE_NAME("safety_button", "driver");
    PRINT_MODULE_USAGE_COMMAND("start");
    PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

    return 0;
}

static int safety_button_main(int argc, char *argv[]) {
    return SafetyButton::main(argc, argv);
}

MSH_CMD_EXPORT_ALIAS(safety_button_main, safety, safety button);

static int safety_button_start() {
    const char *argv[] = {"safety_button", "start"};
    int         argc   = sizeof(argv) / sizeof(argv[0]);

    return SafetyButton::main(argc, (char **)argv);
}

INIT_APP_EXPORT(safety_button_start);
