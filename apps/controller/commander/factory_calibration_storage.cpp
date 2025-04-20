/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <lib/parameters/param.h>
#include <px4_platform_common/log.h>

#include "factory_calibration_storage.h"


static const char *CALIBRATION_STORAGE = "/fs/mtd_caldata";

static bool filter_calibration_params(param_t handle) {
    const char *name = param_get_name(handle);
    // filter all non-calibration params
    return (strncmp(name, "CAL_", 4) == 0 && strncmp(name, "CAL_MAG_SIDES", 13) != 0) || strncmp(name, "TC_", 3) == 0;
}

FactoryCalibrationStorage::FactoryCalibrationStorage() {
    int32_t param = 0;
    param_get(param_find("SYS_FAC_CAL_MODE"), &param);
    _enabled = param == 1;
}

int FactoryCalibrationStorage::open() {
    if (!_enabled) {
        return 0;
    }

    int ret = access(CALIBRATION_STORAGE, R_OK | W_OK);

    if (ret != 0) {
        return -errno;
    }

    PX4_INFO("Storing parameters to factory storage %s", CALIBRATION_STORAGE);
    param_control_autosave(false);
    return 0;
}

int FactoryCalibrationStorage::store() {
    if (!_enabled) {
        return 0;
    }

    // int ret = param_export_internal(CALIBRATION_STORAGE, filter_calibration_params);

    // if (ret != 0) {
    //     PX4_ERR("param export failed (%i)", ret);
    // }

    return 0 /*ret*/;
}

void FactoryCalibrationStorage::cleanup() {
    if (_enabled) {
        param_control_autosave(true);
    }
}
