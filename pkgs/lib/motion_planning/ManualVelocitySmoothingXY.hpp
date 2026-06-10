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
 * @file ManualVelocitySmoothingXY.hpp
 *
 */

#pragma once

#include <motion_planning/VelocitySmoothing.hpp>

#include <matrix/matrix/math.hpp>

using matrix::Vector2f;

class ManualVelocitySmoothingXY final
{
public:
	ManualVelocitySmoothingXY() = default;
	~ManualVelocitySmoothingXY() = default;

	void reset(const Vector2f &accel, const Vector2f &vel, const Vector2f &pos);
	void update(float dt, const Vector2f &velocity_target);

	void setVelSpFeedback(const Vector2f &fb) { _velocity_setpoint_feedback = fb; }

	void setMaxJerk(const float max_jerk)
	{
		_trajectory[0].setMaxJerk(max_jerk);
		_trajectory[1].setMaxJerk(max_jerk);
	}
	float getMaxJerk() const { return _trajectory[0].getMaxJerk(); }

	void setMaxAccel(const float max_accel)
	{
		_trajectory[0].setMaxAccel(max_accel);
		_trajectory[1].setMaxAccel(max_accel);
	}
	float getMaxAccel() const { return _trajectory[0].getMaxAccel(); }

	void setMaxVel(const float max_vel)
	{
		_trajectory[0].setMaxVel(max_vel);
		_trajectory[1].setMaxVel(max_vel);
	}
	float getMaxVel() const { return _trajectory[0].getMaxVel(); }

	Vector2f getCurrentJerk() const { return _state.j; }
	Vector2f getCurrentAcceleration() const { return _state.a; }

	void setCurrentVelocity(const Vector2f &vel)
	{
		_state.v = vel;
		_trajectory[0].setCurrentVelocity(vel(0));
		_trajectory[1].setCurrentVelocity(vel(1));
	}
	Vector2f getCurrentVelocity() const { return _state.v; }

	void setCurrentPosition(const Vector2f &pos)
	{
		_state.x = pos;
		_trajectory[0].setCurrentPosition(pos(0));
		_trajectory[1].setCurrentPosition(pos(1));
		_position_estimate = pos;

		if (_position_lock_active) {
			_position_setpoint_locked = pos;
		}
	}
	Vector2f getCurrentPosition() const { return _position_setpoint_locked; }

	void setCurrentPositionEstimate(const Vector2f &pos) { _position_estimate = pos; }

private:
	void resetPositionLock();
	void updateTrajectories(float dt);
	void checkPositionLock(const Vector2f &velocity_target);
	void updateTrajDurations(const Vector2f &velocity_target);

	VelocitySmoothing _trajectory[2]; ///< Trajectory in x and y directions

	bool _position_lock_active{false};

	Vector2f _position_setpoint_locked{};

	Vector2f _velocity_setpoint_feedback{};
	Vector2f _position_estimate{};

	struct {
		Vector2f j;
		Vector2f a;
		Vector2f v;
		Vector2f x;
	} _state{};
};
