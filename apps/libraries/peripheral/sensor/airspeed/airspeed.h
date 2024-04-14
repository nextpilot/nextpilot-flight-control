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
 * @file airspeed.h
 * Airspeed estimation declarations
 *
 * @author Lorenz Meier <lorenz@px4.io>
 *
 */

#ifndef AIRSPEED_H_
#define AIRSPEED_H_

#include "math.h"

__BEGIN_DECLS

enum AIRSPEED_SENSOR_MODEL {
	AIRSPEED_SENSOR_MODEL_MEMBRANE = 0,
	AIRSPEED_SENSOR_MODEL_SDP3X,
};

enum AIRSPEED_COMPENSATION_MODEL {
	AIRSPEED_COMPENSATION_MODEL_PITOT = 0,
	AIRSPEED_COMPENSATION_MODEL_NO_PITOT = 1,
	AIRSPEED_COMPENSATION_TUBE_PRESSURE_LOSS = 2
};

/**
 * Calculate indicated airspeed (IAS) and correct for friction inside pitot and tube.
 *
 * Note that the indicated airspeed is not the true airspeed because it
 * lacks the air density compensation. Use the calc_true_airspeed functions to get
 * the true airspeed.
 *
 * @param total_pressure pressure inside the pitot/prandtl tube
 * @param static_pressure pressure at the side of the tube/airplane
 * @return indicated airspeed in m/s
 */
__EXPORT float calc_IAS_corrected(enum AIRSPEED_COMPENSATION_MODEL pmodel,
				  enum AIRSPEED_SENSOR_MODEL smodel,
				  float tube_len, float tube_dia_mm, float differential_pressure, float pressure_ambient, float temperature_celsius);

/**
 * Calculate indicated airspeed (IAS).
 *
 * Note that the indicated airspeed is not the true airspeed because it
 * lacks the air density compensation. Use the calc_true_airspeed functions to get
 * the true airspeed.
 *
 * @param total_pressure pressure inside the pitot/prandtl tube
 * @param static_pressure pressure at the side of the tube/airplane
 * @return indicated airspeed in m/s
 */
__EXPORT float calc_IAS(float differential_pressure);

/**
 * Calculate true airspeed (TAS) from calibrated airspeed (CAS).
 *
 * Note that the true airspeed is NOT the groundspeed, because of the effects of wind.
 *
 * @param speed_equivalent current calibrated airspeed
 * @param pressure_ambient pressure at the side of the tube/airplane
 * @param temperature_celsius air temperature in degrees Celsius
 * @return TAS in m/s
 */
__EXPORT float calc_TAS_from_CAS(float speed_indicated, float pressure_ambient,
				 float temperature_celsius);

/**
 * Calculate calibrated airspeed (CAS) from indicated airspeed (IAS).
 *
 * @param speed_indicated current indicated airspeed
 * @param scale scale from IAS to CAS (accounting for instrument and pitot position erros)
 * @return CAS in m/s
 */
__EXPORT float calc_CAS_from_IAS(float speed_indicated, float scale);


/**
 * Directly calculate true airspeed (TAS).
 *
 * Here we assume to have no instrument or pitot position error (IAS = CAS).
 * Note that the true airspeed is NOT the groundspeed, because of the effects of wind.
 *
 * @param total_pressure pressure inside the pitot/prandtl tube
 * @param static_pressure pressure at the side of the tube/airplane
 * @param temperature_celsius air temperature in degrees Celsius
 * @return true airspeed in m/s
 */
__EXPORT float calc_TAS(float total_pressure, float static_pressure, float temperature_celsius);

/**
* Calculates air density.
*
* @param static_pressure ambient pressure in millibar
* @param temperature_celcius air / ambient temperature in Celsius
*/
__EXPORT float get_air_density(float static_pressure, float temperature_celsius);

/**
 * @brief Calculates calibrated airspeed from true airspeed and air density
 *
 * @param speed_true true airspeed [m/s]
 * @param air_density air density [kg/m3]
 * @return calibrated airspeed [m/s]
 */
__EXPORT float calc_calibrated_from_true_airspeed(float speed_true, float air_density);

__END_DECLS

#endif
