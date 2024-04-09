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
 * IMU gyro FFT enable.
 *
 * @boolean
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_INT32(IMU_GYRO_FFT_EN, 0);

/**
 * IMU gyro FFT minimum frequency.
 *
 * @min 1
 * @max 1000
 * @unit Hz
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(IMU_GYRO_FFT_MIN, 30.f);

/**
 * IMU gyro FFT maximum frequency.
 *
 * @min 1
 * @max 1000
 * @unit Hz
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(IMU_GYRO_FFT_MAX, 150.f);

/**
 * IMU gyro FFT length.
 *
 * @value 256 256
 * @value 512 512
 * @value 1024 1024
 * @value 4096 4096
 * @unit Hz
 * @reboot_required true
 * @group Sensors
 */
PARAM_DEFINE_INT32(IMU_GYRO_FFT_LEN, 512);

/**
 * IMU gyro FFT SNR.
 *
 * @min 1
 * @max 30
 * @group Sensors
 */
PARAM_DEFINE_FLOAT(IMU_GYRO_FFT_SNR, 10.f);
