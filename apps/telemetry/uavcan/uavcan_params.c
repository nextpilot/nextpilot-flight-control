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
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 */

/**
 * UAVCAN mode
 *
 *  0 - UAVCAN disabled.
 *  1 - Enables support for UAVCAN sensors without dynamic node ID allocation and firmware update.
 *  2 - Enables support for UAVCAN sensors with dynamic node ID allocation and firmware update.
 *  3 - Enables support for UAVCAN sensors and actuators with dynamic node ID allocation and firmware update. Also sets the motor control outputs to UAVCAN.
 *
 * @min 0
 * @max 3
 * @value 0 Disabled
 * @value 1 Sensors Manual Config
 * @value 2 Sensors Automatic Config
 * @value 3 Sensors and Actuators (ESCs) Automatic Config
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_ENABLE, 0);

/**
 * UAVCAN Node ID.
 *
 * Read the specs at http://uavcan.org to learn more about Node ID.
 *
 * @min 1
 * @max 125
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_NODE_ID, 1);

/**
 * UAVCAN CAN bus bitrate.
 *
 * @unit bit/s
 * @min 20000
 * @max 1000000
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_BITRATE, 1000000);

/**
 * UAVCAN rangefinder minimum range
 *
 * This parameter defines the minimum valid range for a rangefinder connected via UAVCAN.
 *
 * @unit m
 * @group UAVCAN
 */
PARAM_DEFINE_FLOAT(UAVCAN_RNG_MIN, 0.3f);

/**
 * UAVCAN rangefinder maximum range
 *
 * This parameter defines the maximum valid range for a rangefinder connected via UAVCAN.
 *
 * @unit m
 * @group UAVCAN
 */
PARAM_DEFINE_FLOAT(UAVCAN_RNG_MAX, 200.0f);

/**
 * UAVCAN ANTI_COLLISION light operating mode
 *
 * This parameter defines the minimum condition under which the system will command
 * the ANTI_COLLISION lights on
 *
 *  0 - Always off
 *  1 - When autopilot is armed
 *  2 - When autopilot is prearmed
 *  3 - Always on
 *
 * @min 0
 * @max 3
 * @value 0 Always off
 * @value 1 When autopilot is armed
 * @value 2 When autopilot is prearmed
 * @value 3 Always on
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_LGT_ANTCL, 2);

/**
 * UAVCAN STROBE light operating mode
 *
 * This parameter defines the minimum condition under which the system will command
 * the STROBE lights on
 *
 *  0 - Always off
 *  1 - When autopilot is armed
 *  2 - When autopilot is prearmed
 *  3 - Always on
 *
 * @min 0
 * @max 3
 * @value 0 Always off
 * @value 1 When autopilot is armed
 * @value 2 When autopilot is prearmed
 * @value 3 Always on
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_LGT_STROB, 1);

/**
 * UAVCAN RIGHT_OF_WAY light operating mode
 *
 * This parameter defines the minimum condition under which the system will command
 * the RIGHT_OF_WAY lights on
 *
 *  0 - Always off
 *  1 - When autopilot is armed
 *  2 - When autopilot is prearmed
 *  3 - Always on
 *
 * @min 0
 * @max 3
 * @value 0 Always off
 * @value 1 When autopilot is armed
 * @value 2 When autopilot is prearmed
 * @value 3 Always on
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_LGT_NAV, 3);

/**
 * UAVCAN LIGHT_ID_LANDING light operating mode
 *
 * This parameter defines the minimum condition under which the system will command
 * the LIGHT_ID_LANDING lights on
 *
 *  0 - Always off
 *  1 - When autopilot is armed
 *  2 - When autopilot is prearmed
 *  3 - Always on
 *
 * @min 0
 * @max 3
 * @value 0 Always off
 * @value 1 When autopilot is armed
 * @value 2 When autopilot is prearmed
 * @value 3 Always on
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_LGT_LAND, 0);

/**
 * publish Arming Status stream
 *
 * Enable UAVCAN Arming Status stream publication
 *  uavcan::equipment::safety::ArmingStatus
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_PUB_ARM, 0);

/**
 * publish RTCM stream
 *
 * Enable UAVCAN RTCM stream publication
 *  uavcan::equipment::gnss::RTCMStream
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_PUB_RTCM, 0);

/**
 * publish moving baseline data RTCM stream
 *
 * Enable UAVCAN RTCM stream publication
 *  ardupilot::gnss::MovingBaselineData
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_PUB_MBD, 0);

/**
 * subscription airspeed
 *
 * Enable UAVCAN airspeed subscriptions.
 *  uavcan::equipment::air_data::IndicatedAirspeed
 *  uavcan::equipment::air_data::TrueAirspeed
 *  uavcan::equipment::air_data::StaticTemperature
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_ASPD, 0);

/**
 * subscription barometer
 *
 * Enable UAVCAN barometer subscription.
 *  uavcan::equipment::air_data::StaticPressure
 *  uavcan::equipment::air_data::StaticTemperature
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_BARO, 0);

/**
 * subscription battery
 *
 * Enable UAVCAN battery subscription.
 *  uavcan::equipment::power::BatteryInfo
 *  ardupilot::equipment::power::BatteryInfoAux
 *
 *  0 - Disable
 *  1 - Use raw data. Recommended for Smart battery
 *  2 - Filter the data with internal battery library
 *
 * @min 0
 * @max 2
 * @value 0 Disable
 * @value 1 Raw data
 * @value 2 Filter data
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_BAT, 0);

/**
 * subscription differential pressure
 *
 * Enable UAVCAN differential pressure subscription.
 *  uavcan::equipment::air_data::RawAirData
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_DPRES, 0);

/**
 * subscription flow
 *
 * Enable UAVCAN optical flow subscription.
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_FLOW, 0);

/**
 * subscription GPS
 *
 * Enable UAVCAN GPS subscriptions.
 *  uavcan::equipment::gnss::Fix
 *  uavcan::equipment::gnss::Fix2
 *  uavcan::equipment::gnss::Auxiliary
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_GPS, 1);

/**
 * subscription hygrometer
 *
 * Enable UAVCAN hygrometer subscriptions.
 *  dronecan::sensors::hygrometer::Hygrometer
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_HYGRO, 0);

/**
 * subscription ICE
 *
 * Enable UAVCAN internal combustion engine (ICE) subscription.
 *  uavcan::equipment::ice::reciprocating::Status
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_ICE, 0);

/**
 * subscription IMU
 *
 * Enable UAVCAN IMU subscription.
 *  uavcan::equipment::ahrs::RawIMU
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_IMU, 0);

/**
 * subscription magnetometer
 *
 * Enable UAVCAN mag subscription.
 *  uavcan::equipment::ahrs::MagneticFieldStrength
 *  uavcan::equipment::ahrs::MagneticFieldStrength2
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_MAG, 1);

/**
 * subscription range finder
 *
 * Enable UAVCAN range finder subscription.
 *  uavcan::equipment::range_sensor::Measurement
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_RNG, 0);

/**
 * subscription button
 *
 * Enable UAVCAN button subscription.
 *  ardupilot::indication::Button
 *
 * @boolean
 * @reboot_required true
 * @group UAVCAN
 */
PARAM_DEFINE_INT32(UAVCAN_SUB_BTN, 0);
