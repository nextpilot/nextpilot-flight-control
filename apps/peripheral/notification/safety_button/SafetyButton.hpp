/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __SAFETYBUTTON_H__
#define __SAFETYBUTTON_H__

#include <float.h>
#include <rtdevice.h>
#include <hrtimer.h>
#include <module/module_command.hpp>
#include <module/module_params.hpp>
#include <workq/WorkItemScheduled.hpp>
#include <button/ButtonPublisher.hpp>

class SafetyButton : public ModuleCommand<SafetyButton>, public nextpilot::WorkItemScheduled {
public:
    SafetyButton();
    ~SafetyButton() override;

    /** @see ModuleCommand */
    // static int task_spawn(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int custom_command(int argc, char *argv[]);

    /** @see ModuleCommand */
    static int print_usage(const char *reason = nullptr);

    int init() override;

private:
    void Run() override;

    void CheckSafetyRequest(bool button_pressed);
    void CheckPairingRequest(bool button_pressed);
    void FlashButton();

    bool            _has_px4io{false};
    ButtonPublisher _button_publisher;
    uint8_t         _button_counter{0};
    uint8_t         _blink_counter{0};
    bool            _button_prev_sate{false}; ///< Previous state of the HW button

    // Pairing request
    hrt_abstime _pairing_start{0};
    int         _pairing_button_counter{0};

    uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};
};

#endif // __SAFETYBUTTON_H__
