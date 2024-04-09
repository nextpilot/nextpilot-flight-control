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

#include "../Common.hpp"

class SdCardChecks : public HealthAndArmingCheckBase {
public:
    SdCardChecks()  = default;
    ~SdCardChecks() = default;

    void checkAndReport(const Context &context, Report &reporter) override;

private:
#ifdef PX4_STORAGEDIR
    bool _sdcard_detected{false};

#ifdef __PX4_NUTTX
    bool _hardfault_checked_once{false};
    bool _hardfault_file_present{false};
#endif
#endif

    DEFINE_PARAMETERS_CUSTOM_PARENT(HealthAndArmingCheckBase,
                                    (ParamInt<px4::params::COM_ARM_SDCARD>)_param_com_arm_sdcard,
                                    (ParamBool<px4::params::COM_ARM_HFLT_CHK>)_param_com_arm_hardfault_check)
};
