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

#include "input.h"
#include <px4_platform_common/atomic.h>

namespace gimbal {

class InputTest : public InputBase {
public:
    InputTest() = delete;
    explicit InputTest(Parameters &parameters);
    virtual ~InputTest() = default;

    UpdateResult update(unsigned int timeout_ms, ControlData &control_data, bool already_active) override;
    int          initialize() override;
    void         print_status() const override;

    void set_test_input(int roll_deg, int pitch_deg, int yaw_deg);

private:
    int _roll_deg{0};
    int _pitch_deg{0};
    int _yaw_deg{0};

    px4::atomic<bool> _has_been_set{false};
};

} /* namespace gimbal */
