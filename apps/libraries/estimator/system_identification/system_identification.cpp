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
 * @file system_identification.cpp
 *
 * @author Mathieu Bresciani <mathieu@auterion.com>
 */

#include "system_identification.hpp"

void SystemIdentification::reset(const matrix::Vector<float, 5> &id_state_init)
{
	_rls.reset(id_state_init);
	_u_lpf.reset(0.f);
	_u_lpf.reset(0.f);
	_u_hpf = 0.f;
	_y_hpf = 0.f;
	_u_prev = 0.f;
	_y_prev = 0.f;
	_fitness_lpf.reset(10.f);
	_are_filters_initialized = false;
}

void SystemIdentification::update(float u, float y)
{
	updateFilters(u, y);
	update();
}

void SystemIdentification::update()
{
	_rls.update(_u_hpf, _y_hpf);
	updateFitness();
}

void SystemIdentification::updateFilters(float u, float y)
{
	if (!_are_filters_initialized) {
		_u_lpf.reset(u);
		_y_lpf.reset(y);
		_u_hpf = 0.f;
		_y_hpf = 0.f;
		_u_prev = u;
		_y_prev = y;
		_are_filters_initialized = true;
		return;
	}

	const float u_lpf = _u_lpf.apply(u);
	const float y_lpf = _y_lpf.apply(y);
	_u_hpf = _alpha_hpf * _u_hpf + _alpha_hpf * (u_lpf - _u_prev);
	_y_hpf = _alpha_hpf * _y_hpf + _alpha_hpf * (y_lpf - _y_prev);

	_u_prev = u_lpf;
	_y_prev = y_lpf;
}

void SystemIdentification::updateFitness()
{
	const matrix::Vector<float, 5> &diff = _rls.getDiffEstimate();
	float sum = 0.f;

	for (size_t i = 0; i < 5; i++) {
		sum += diff(i);
	}

	if (_dt > FLT_EPSILON) {
		_fitness_lpf.update(sum / _dt);
	}
}
