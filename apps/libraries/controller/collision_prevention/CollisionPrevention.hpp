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
 * @file CollisionPrevention.hpp
 * @author Tanja Baumann <tanja@auterion.com>
 *
 * CollisionPrevention controller.
 *
 */

#pragma once

#include <float.h>

#include <commander/px4_custom_mode.h>
#include <drivers/drv_hrt.h>
#include <mathlib/mathlib.h>
#include <matrix/matrix/math.hpp>
#include <px4_platform_common/module_params.h>
#include <systemlib/mavlink_log.h>
#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionMultiArray.hpp>
#include <uORB/topics/collision_constraints.h>
#include <uORB/topics/distance_sensor.h>
#include <uORB/topics/mavlink_log.h>
#include <uORB/topics/obstacle_distance.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/vehicle_command.h>

using namespace time_literals;

class CollisionPrevention : public ModuleParams
{
public:
	CollisionPrevention(ModuleParams *parent);
	~CollisionPrevention() override = default;

	/**
	 * Returns true if Collision Prevention is running
	 */
	bool is_active();

	/**
	 * Computes collision free setpoints
	 * @param original_setpoint, setpoint before collision prevention intervention
	 * @param max_speed, maximum xy speed
	 * @param curr_pos, current vehicle position
	 * @param curr_vel, current vehicle velocity
	 */
	void modifySetpoint(matrix::Vector2f &original_setpoint, const float max_speed,
			    const matrix::Vector2f &curr_pos, const matrix::Vector2f &curr_vel);

protected:

	obstacle_distance_s _obstacle_map_body_frame {};
	bool _data_fov[sizeof(_obstacle_map_body_frame.distances) / sizeof(_obstacle_map_body_frame.distances[0])];
	uint64_t _data_timestamps[sizeof(_obstacle_map_body_frame.distances) / sizeof(_obstacle_map_body_frame.distances[0])];
	uint16_t _data_maxranges[sizeof(_obstacle_map_body_frame.distances) / sizeof(
										    _obstacle_map_body_frame.distances[0])]; /**< in cm */

	void _addDistanceSensorData(distance_sensor_s &distance_sensor, const matrix::Quatf &vehicle_attitude);

	/**
	 * Updates obstacle distance message with measurement from offboard
	 * @param obstacle, obstacle_distance message to be updated
	 */
	void _addObstacleSensorData(const obstacle_distance_s &obstacle, const matrix::Quatf &vehicle_attitude);

	/**
	 * Computes an adaption to the setpoint direction to guide towards free space
	 * @param setpoint_dir, setpoint direction before collision prevention intervention
	 * @param setpoint_index, index of the setpoint in the internal obstacle map
	 * @param vehicle_yaw_angle_rad, vehicle orientation
	 */
	void _adaptSetpointDirection(matrix::Vector2f &setpoint_dir, int &setpoint_index, float vehicle_yaw_angle_rad);

	/**
	 * Determines whether a new sensor measurement is used
	 * @param map_index, index of the bin in the internal map the measurement belongs in
	 * @param sensor_range, max range of the sensor in meters
	 * @param sensor_reading, distance measurement in meters
	 */
	bool _enterData(int map_index, float sensor_range, float sensor_reading);


	//Timing functions. Necessary to mock time in the tests
	virtual hrt_abstime getTime();
	virtual hrt_abstime getElapsedTime(const hrt_abstime *ptr);


private:

	bool _interfering{false};		/**< states if the collision prevention interferes with the user input */
	bool _was_active{false};		/**< states if the collision prevention interferes with the user input */

	orb_advert_t _mavlink_log_pub{nullptr};	 	/**< Mavlink log uORB handle */

	uORB::Publication<collision_constraints_s>	_constraints_pub{ORB_ID(collision_constraints)};		/**< constraints publication */
	uORB::Publication<obstacle_distance_s>		_obstacle_distance_pub{ORB_ID(obstacle_distance_fused)};	/**< obstacle_distance publication */
	uORB::Publication<vehicle_command_s>	_vehicle_command_pub{ORB_ID(vehicle_command)};			/**< vehicle command do publication */

	uORB::SubscriptionData<obstacle_distance_s> _sub_obstacle_distance{ORB_ID(obstacle_distance)}; /**< obstacle distances received form a range sensor */
	uORB::SubscriptionData<vehicle_attitude_s> _sub_vehicle_attitude{ORB_ID(vehicle_attitude)};
	uORB::SubscriptionMultiArray<distance_sensor_s> _distance_sensor_subs{ORB_ID::distance_sensor};

	static constexpr uint64_t RANGE_STREAM_TIMEOUT_US{500_ms};
	static constexpr uint64_t TIMEOUT_HOLD_US{5_s};

	hrt_abstime	_last_timeout_warning{0};
	hrt_abstime	_time_activated{0};

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::CP_DIST>) _param_cp_dist, /**< collision prevention keep minimum distance */
		(ParamFloat<px4::params::CP_DELAY>) _param_cp_delay, /**< delay of the range measurement data*/
		(ParamFloat<px4::params::CP_GUIDE_ANG>) _param_cp_guide_ang, /**< collision prevention change setpoint angle */
		(ParamBool<px4::params::CP_GO_NO_DATA>) _param_cp_go_nodata, /**< movement allowed where no data*/
		(ParamFloat<px4::params::MPC_XY_P>) _param_mpc_xy_p, /**< p gain from position controller*/
		(ParamFloat<px4::params::MPC_JERK_MAX>) _param_mpc_jerk_max, /**< vehicle maximum jerk*/
		(ParamFloat<px4::params::MPC_ACC_HOR>) _param_mpc_acc_hor /**< vehicle maximum horizontal acceleration*/
	)

	/**
	 * Transforms the sensor orientation into a yaw in the local frame
	 * @param distance_sensor, distance sensor message
	 * @param angle_offset, sensor body frame offset
	 */
	float _sensorOrientationToYawOffset(const distance_sensor_s &distance_sensor, float angle_offset) const;

	/**
	 * Computes collision free setpoints
	 * @param setpoint, setpoint before collision prevention intervention
	 * @param curr_pos, current vehicle position
	 * @param curr_vel, current vehicle velocity
	 */
	void _calculateConstrainedSetpoint(matrix::Vector2f &setpoint, const matrix::Vector2f &curr_pos,
					   const matrix::Vector2f &curr_vel);

	/**
	 * Publishes collision_constraints message
	 * @param original_setpoint, setpoint before collision prevention intervention
	 * @param adapted_setpoint, collision prevention adaped setpoint
	 */
	void _publishConstrainedSetpoint(const matrix::Vector2f &original_setpoint, const matrix::Vector2f &adapted_setpoint);

	/**
	 * Publishes obstacle_distance message with fused data from offboard and from distance sensors
	 * @param obstacle, obstacle_distance message to be publsihed
	 */
	void _publishObstacleDistance(obstacle_distance_s &obstacle);

	/**
	 * Aggregates the sensor data into a internal obstacle map in body frame
	 */
	void _updateObstacleMap();

	/**
	 * Publishes vehicle command.
	 */
	void _publishVehicleCmdDoLoiter();

};
