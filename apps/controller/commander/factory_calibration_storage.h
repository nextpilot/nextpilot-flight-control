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

/**
 * @class FactoryCalibrationStorage
 * Stores calibration parameters to a separate storage, if enabled by parameter
 */
class FactoryCalibrationStorage {
public:
    FactoryCalibrationStorage();
    ~FactoryCalibrationStorage() {
        cleanup();
    }

    /**
     * open the storage & disable param autosaving
     * @return 0 on success, <0 error otherwise
     */
    int open();

    /**
     * store the calibration parameters
     * Note: this method requires a lot of stack
     * @return 0 on success, <0 error otherwise
     */
    int store();

private:
    void cleanup();

    bool _enabled{false};
};
