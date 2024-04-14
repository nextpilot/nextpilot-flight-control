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

#include "common.h"
#include "math.h"
#include "gimbal_params.h"

namespace gimbal {

struct Parameters;

class InputBase {
public:
    enum class UpdateResult {
        NoUpdate,
        UpdatedActive,
        UpdatedActiveOnce,
        UpdatedNotActive,
    };

    InputBase() = delete;
    explicit InputBase(Parameters &parameters);
    virtual ~InputBase() = default;

    virtual int          initialize()                                                                    = 0;
    virtual UpdateResult update(unsigned int timeout_ms, ControlData &control_data, bool already_active) = 0;
    virtual void         print_status() const                                                            = 0;

protected:
    void control_data_set_lon_lat(ControlData &control_data, double lon, double lat, float altitude, float roll_angle = NAN,
                                  float pitch_fixed_angle = NAN);

    Parameters &_parameters;
};

} /* namespace gimbal */
