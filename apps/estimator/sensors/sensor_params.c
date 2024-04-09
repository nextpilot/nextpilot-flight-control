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
 * Airspeed sensor compensation model for the SDP3x
 *
 * Model with Pitot
 * 		CAL_AIR_TUBED_MM: Not used, 1.5 mm tubes assumed.
 * 		CAL_AIR_TUBELEN: Length of the tubes connecting the pitot to the sensor.
 * Model without Pitot (1.5 mm tubes)
 * 		CAL_AIR_TUBED_MM: Not used, 1.5 mm tubes assumed.
 * 		CAL_AIR_TUBELEN: Length of the tubes connecting the pitot to the sensor.
 * Tube Pressure Drop
 * 		CAL_AIR_TUBED_MM: Diameter in mm of the pitot and tubes, must have the same diameter.
 * 		CAL_AIR_TUBELEN: Length of the tubes connecting the pitot to the sensor and the static + dynamic port length of the pitot.
 *
 * @value 0 Model with Pitot
 * @value 1 Model without Pitot (1.5 mm tubes)
 * @value 2 Tube Pressure Drop
 *
 * @group Sensors
 */
PARAM_DEFINE_INT32(CAL_AIR_CMODEL, 0);

/**
 * Airspeed sensor tube length.
 *
 * See the CAL_AIR_CMODEL explanation on how this parameter should be set.
 *
 * @min 0.01
 * @max 2.00
 * @unit m
 *
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(CAL_AIR_TUBELEN, 0.2f);

/**
 * Airspeed sensor tube diameter. Only used for the Tube Pressure Drop Compensation.
 *
 * @min 1.5
 * @max 100
 * @unit mm
 *
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(CAL_AIR_TUBED_MM, 1.5f);

/**
 * Differential pressure sensor offset
 *
 * The offset (zero-reading) in Pascal
 *
 * @category system
 * @group Sensor Calibration
 * @volatile
 */
PARAM_DEFINE_FLOAT(SENS_DPRES_OFF, 0.0f);

/**
 * Differential pressure sensor analog scaling
 *
 * Pick the appropriate scaling from the datasheet.
 * this number defines the (linear) conversion from voltage
 * to Pascal (pa). For the MPXV7002DP this is 1000.
 *
 * NOTE: If the sensor always registers zero, try switching
 * the static and dynamic tubes.
 *
 * @group Sensor Calibration
 */
PARAM_DEFINE_FLOAT(SENS_DPRES_ANSC, 0);

/**
 * Board rotation
 *
 * This parameter defines the rotation of the FMU board relative to the platform.
 *
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 *
 * @min -1
 * @max 40
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_BOARD_ROT, 0);

/**
 * Board rotation Y (Pitch) offset
 *
 * This parameter defines a rotational offset in degrees around the Y (Pitch) axis. It allows the user
 * to fine tune the board offset in the event of misalignment.
 *
 * @unit deg
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(SENS_BOARD_Y_OFF, 0.0f);

/**
 * Board rotation X (Roll) offset
 *
 * This parameter defines a rotational offset in degrees around the X (Roll) axis It allows the user
 * to fine tune the board offset in the event of misalignment.
 *
 * @unit deg
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(SENS_BOARD_X_OFF, 0.0f);

/**
 * Board rotation Z (YAW) offset
 *
 * This parameter defines a rotational offset in degrees around the Z (Yaw) axis. It allows the user
 * to fine tune the board offset in the event of misalignment.
 *
 * @unit deg
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(SENS_BOARD_Z_OFF, 0.0f);

/**
 * Thermal control of sensor temperature
 *
 * @value -1 Thermal control unavailable
 * @value 0 Thermal control off
 * @value 1 Thermal control enabled
 * @category system
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_EN_THERMAL, -1);

/**
 * External I2C probe.
 *
 * Probe for optional external I2C devices.
 *
 * @boolean
 * @category system
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_EXT_I2C_PRB, 1);

/**
 * Sensors hub IMU mode
 *
 * @value 0 Disabled
 * @value 1 Publish primary IMU selection
 *
 * @category system
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_IMU_MODE, 1);

/**
 * Enable internal barometers
 *
 * For systems with an external barometer, this should be set to false to make sure that the external is used.
 *
 * @boolean
 * @reboot_required true
 * @category system
 * @group Sensors
 */
PARAM_DEFINE_INT32(SENS_INT_BARO_EN, 1);
