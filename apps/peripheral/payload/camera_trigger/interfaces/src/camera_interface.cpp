/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "camera_interface.h"
#include <px4_platform_common/log.h>
#include <board_config.h>

void CameraInterface::get_pins() {
    // Set all pins as invalid
    for (unsigned i = 0; i < arraySize(_pins); i++) {
        _pins[i] = -1;
    }

    unsigned pin_index = 0;

    for (unsigned i = 0; i < 16 && pin_index < arraySize(_pins); ++i) {
        char param_name[17];
        snprintf(param_name, sizeof(param_name), "%s_%s%d", PARAM_PREFIX, "FUNC", i + 1);
        param_t function_handle = param_find(param_name);
        int32_t function;

        if (function_handle != PARAM_INVALID && param_get(function_handle, &function) == 0) {
            if (function == 2000) { // Camera_Trigger
                _pins[pin_index++] = i;
            }
        }
    }
}
