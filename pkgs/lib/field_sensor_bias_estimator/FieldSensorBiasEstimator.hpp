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
 * @file FieldSensorBiasEstimator.hpp
 *
 * Estimator for the magnetometer/accelerometer bias calibration parameters to run online with the help of Gyroscope data.
 *
 * @author Matthias Grob	<maetugr@gmail.com>
 * @author Mathieu Bresciani	<brescianimathieu@gmail.com>
 *
 * Publication documenting the algorithm:
 * Adaptive Estimation of Measurement Bias in Three-Dimensional Field Sensors with Angular-Rate Sensors: Theory and Comparative Experimental Evaluation
 * Giancarlo Troni and Louis L. Whitcomb, Department of Mechanical Engineering, Johns Hopkins University, Baltimore, Maryland 21218, USA
 *
 * http://www.roboticsproceedings.org/rss09/p50.pdf
 */

#pragma once

#include <matrix/matrix/math.hpp>

class FieldSensorBiasEstimator
{
public:
	FieldSensorBiasEstimator() = default;
	~FieldSensorBiasEstimator() = default;

	// Set initial states
	void setField(const matrix::Vector3f &field) { _field_prev = field; }
	void setBias(const matrix::Vector3f &bias) { _state_bias = bias; }
	void setLearningGain(float learning_gain) { _learning_gain = learning_gain; }

	/**
	 * Update the estimator and extract updated biases.
	 * @param gyro bias corrected gyroscope data in the same coordinate frame as the field sensor data
	 * @param field biased field sensor data
	 * @param dt time in seconds since the last update
	 */
	void updateEstimate(const matrix::Vector3f &gyro, const matrix::Vector3f &field, const float dt)
	{
		const matrix::Vector3f field_pred = _field_prev + (-gyro % (_field_prev - _state_bias)) * dt;
		const matrix::Vector3f field_innov = field - field_pred;
		_state_bias += _learning_gain * (-gyro % field_innov) * dt;
		_field_prev = field;
	}

	const matrix::Vector3f &getField() { return _field_prev; }
	const matrix::Vector3f &getBias() { return _state_bias; }

private:
	// states
	matrix::Vector3f _field_prev{};
	matrix::Vector3f _state_bias{};
	float _learning_gain{20.f};
};
