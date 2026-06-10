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
 * @file ManualVelocitySmoothingZ.hpp
 *
 */

#pragma once

#include <motion_planning/VelocitySmoothing.hpp>

class ManualVelocitySmoothingZ final
{
public:
	ManualVelocitySmoothingZ() = default;
	~ManualVelocitySmoothingZ() = default;

	void reset(float accel, float vel, float pos);
	void update(float dt, float velocity_target);

	void setVelSpFeedback(const float fb) { _velocity_setpoint_feedback = fb; }

	void setMaxJerk(const float max_jerk)
	{
		_trajectory.setMaxJerk(max_jerk);
	}
	void setMaxAccelUp(const float max_accel_up)
	{
		_max_accel_up = max_accel_up;
	}
	void setMaxAccelDown(const float max_accel_down)
	{
		_max_accel_down = max_accel_down;
	}
	void setMaxVelUp(const float max_vel_up)
	{
		_max_vel_up = max_vel_up;
	}
	void setMaxVelDown(const float max_vel_down)
	{
		_max_vel_down = max_vel_down;
	}

	float getCurrentJerk() const { return _state.j; }
	float getCurrentAcceleration() const { return _state.a; }
	void setCurrentVelocity(const float vel)
	{
		_state.v = vel;
		_trajectory.setCurrentVelocity(vel);
	}
	float getCurrentVelocity() const { return _state.v; }
	void setCurrentPosition(const float pos)
	{
		_state.x = pos;
		_trajectory.setCurrentPosition(pos);
		_position_estimate = pos;

		if (_position_lock_active) {
			_position_setpoint_locked = pos;
		}
	}
	float getCurrentPosition() const { return _position_setpoint_locked; }
	void setCurrentPositionEstimate(float pos) { _position_estimate = pos; }

private:
	void resetPositionLock();
	void updateTrajectories(float dt);
	void updateTrajConstraints(float vel_target);
	void checkPositionLock(float velocity_target);
	void updateTrajDurations(float velocity_target);

	VelocitySmoothing _trajectory; ///< Trajectory in z direction

	bool _position_lock_active{false};

	float _position_setpoint_locked{};

	float _velocity_setpoint_feedback{};
	float _position_estimate{};

	struct {
		float j;
		float a;
		float v;
		float x;
	} _state{};

	float _max_accel_up{0.f};
	float _max_accel_down{0.f};
	float _max_vel_up{0.f};
	float _max_vel_down{0.f};
};
