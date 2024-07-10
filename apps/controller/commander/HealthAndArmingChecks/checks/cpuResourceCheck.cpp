/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "cpuload_check"
#define LOG_LVL LOG_LVL_INFO

#include "cpuResourceCheck.hpp"

using namespace time_literals;

void CpuResourceChecks::checkAndReport(const Context &context, Report &reporter) {
#ifdef BSP_USING_QEMU
    return;
#endif /* BSP_USING_QEMU */

    if (_param_com_cpu_max.get() < FLT_EPSILON) {
        return;
    }

    cpuload_s cpuload;

    if (!_cpuload_sub.copy(&cpuload) || hrt_elapsed_time(&cpuload.timestamp) > 2_s) {
        /* EVENT
         * @description
         * <profile name="dev">
         * If the system does not provide any CPU load information, use the parameter <param>COM_CPU_MAX</param>
         * to disable the check.
         * </profile>
         */
        reporter.healthFailure(NavModes::All, health_component_t::system, events::ID("check_missing_cpuload"),
                               events::Log::Error, "No CPU load information");

        if (reporter.mavlink_log_pub()) {
            mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: No CPU load information");
        }

    } else {
        const float cpuload_percent = cpuload.load * 100.f;

        if (cpuload_percent > _param_com_cpu_max.get()) {
            /* EVENT
             * @description
             * The CPU load can be reduced for example by disabling unused modules (e.g. mavlink instances) or reducing the gyro update
             * rate via <param>IMU_GYRO_RATEMAX</param>.
             *
             * <profile name="dev">
             * The threshold can be adjusted via <param>COM_CPU_MAX</param> parameter.
             * </profile>
             */
            reporter.healthFailure<float>(NavModes::All, health_component_t::system, events::ID("check_cpuload_too_high"),
                                          events::Log::Error, "CPU load too high: {1:.1}%", cpuload_percent);

            if (reporter.mavlink_log_pub()) {
                mavlink_log_critical(reporter.mavlink_log_pub(), "Preflight Fail: CPU load too high: %3.1f%%", (double)cpuload_percent);
            }
        }
    }
}
