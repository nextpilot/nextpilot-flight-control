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
 * Class to write EKF state to file
 * @author Kamil Ritz <ka.ritz@hotmail.com>
 */
#ifndef EKF_EKF_LOGGER_H
#define EKF_EKF_LOGGER_H

#include "EKF/ekf.h"
#include "EKF/estimator_interface.h"
#include "ekf_wrapper.h"
#include <fstream>
#include <iostream>

class EkfLogger {
public:
    EkfLogger(std::shared_ptr<Ekf> ekf);
    ~EkfLogger() = default;
    void setFilePath(std::string file_path);

    void enableStateLogging() {
        _state_logging_enabled = true;
    };
    void disableStateLogging() {
        _state_logging_enabled = false;
    };
    void enableVarianceLogging() {
        _variance_logging_enabled = true;
    };
    void disableVarianceLogging() {
        _variance_logging_enabled = false;
    };

    void writeStateToFile();

private:
    std::shared_ptr<Ekf> _ekf;
    EkfWrapper           _ekf_wrapper;

    std::string   _file_path;
    std::ofstream _file;

    bool _file_opened{false};

    bool _state_logging_enabled{true};
    bool _variance_logging_enabled{true};

    void writeState();
};
#endif // !EKF_EKF_LOGGER_H
