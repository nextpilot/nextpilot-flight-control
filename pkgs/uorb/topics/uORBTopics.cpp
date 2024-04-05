
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <topics/uORBTopics.hpp>
#include <uORB.h>
#include <topics/action_request.h>
#include <topics/actuator_armed.h>
#include <topics/actuator_controls_status.h>
#include <topics/actuator_motors.h>
#include <topics/actuator_outputs.h>
#include <topics/actuator_servos.h>
#include <topics/actuator_servos_trim.h>
#include <topics/actuator_test.h>
#include <topics/adc_report.h>
#include <topics/airspeed.h>
#include <topics/airspeed_validated.h>
#include <topics/airspeed_wind.h>
#include <topics/autotune_attitude_control_status.h>
#include <topics/battery_status.h>
#include <topics/button_event.h>
#include <topics/camera_capture.h>
#include <topics/camera_status.h>
#include <topics/camera_trigger.h>
#include <topics/cellular_status.h>
#include <topics/collision_constraints.h>
#include <topics/collision_report.h>
#include <topics/control_allocator_status.h>
#include <topics/cpuload.h>
#include <topics/debug_array.h>
#include <topics/debug_key_value.h>
#include <topics/debug_value.h>
#include <topics/debug_vect.h>
#include <topics/differential_pressure.h>
#include <topics/distance_sensor.h>
#include <topics/ekf2_timestamps.h>
#include <topics/esc_report.h>
#include <topics/esc_status.h>
#include <topics/estimator_aid_source1d.h>
#include <topics/estimator_aid_source2d.h>
#include <topics/estimator_aid_source3d.h>
#include <topics/estimator_bias.h>
#include <topics/estimator_bias3d.h>
#include <topics/estimator_event_flags.h>
#include <topics/estimator_gps_status.h>
#include <topics/estimator_innovations.h>
#include <topics/estimator_selector_status.h>
#include <topics/estimator_sensor_bias.h>
#include <topics/estimator_states.h>
#include <topics/estimator_status.h>
#include <topics/estimator_status_flags.h>
#include <topics/event.h>
#include <topics/failsafe_flags.h>
#include <topics/failure_detector_status.h>
#include <topics/follow_target.h>
#include <topics/follow_target_estimator.h>
#include <topics/follow_target_status.h>
#include <topics/generator_status.h>
#include <topics/geofence_result.h>
#include <topics/gimbal_controls.h>
#include <topics/gimbal_device_attitude_status.h>
#include <topics/gimbal_device_information.h>
#include <topics/gimbal_device_set_attitude.h>
#include <topics/gimbal_manager_information.h>
#include <topics/gimbal_manager_set_attitude.h>
#include <topics/gimbal_manager_set_manual_control.h>
#include <topics/gimbal_manager_status.h>
#include <topics/gps_dump.h>
#include <topics/gps_inject_data.h>
#include <topics/gripper.h>
#include <topics/health_report.h>
#include <topics/heater_status.h>
#include <topics/home_position.h>
#include <topics/hover_thrust_estimate.h>
#include <topics/input_rc.h>
#include <topics/internal_combustion_engine_status.h>
#include <topics/iridiumsbd_status.h>
#include <topics/irlock_report.h>
#include <topics/landing_gear.h>
#include <topics/landing_gear_wheel.h>
#include <topics/landing_target_innovations.h>
#include <topics/landing_target_pose.h>
#include <topics/launch_detection_status.h>
#include <topics/led_control.h>
#include <topics/log_message.h>
#include <topics/logger_status.h>
#include <topics/mag_worker_data.h>
#include <topics/magnetometer_bias_estimate.h>
#include <topics/manual_control_setpoint.h>
#include <topics/manual_control_switches.h>
#include <topics/mavlink_log.h>
#include <topics/mavlink_tunnel.h>
#include <topics/mission.h>
#include <topics/mission_result.h>
#include <topics/mode_completed.h>
#include <topics/mount_orientation.h>
#include <topics/navigator_mission_item.h>
#include <topics/normalized_unsigned_setpoint.h>
#include <topics/npfg_status.h>
#include <topics/obstacle_distance.h>
#include <topics/offboard_control_mode.h>
#include <topics/onboard_computer_status.h>
#include <topics/orb_test.h>
#include <topics/orb_test_large.h>
#include <topics/orb_test_medium.h>
#include <topics/orbit_status.h>
#include <topics/parameter_update.h>
#include <topics/ping.h>
#include <topics/position_controller_landing_status.h>
#include <topics/position_controller_status.h>
#include <topics/position_setpoint.h>
#include <topics/position_setpoint_triplet.h>
#include <topics/power_button_state.h>
#include <topics/power_monitor.h>
#include <topics/pps_capture.h>
#include <topics/pwm_input.h>
#include <topics/px4io_status.h>
#include <topics/qshell_req.h>
#include <topics/qshell_retval.h>
#include <topics/radio_status.h>
#include <topics/rate_ctrl_status.h>
#include <topics/rc_channels.h>
#include <topics/rc_parameter_map.h>
#include <topics/rpm.h>
#include <topics/rtl_time_estimate.h>
#include <topics/satellite_info.h>
#include <topics/sensor_accel.h>
#include <topics/sensor_accel_fifo.h>
#include <topics/sensor_baro.h>
#include <topics/sensor_combined.h>
#include <topics/sensor_correction.h>
#include <topics/sensor_gnss_relative.h>
#include <topics/sensor_gps.h>
#include <topics/sensor_gyro.h>
#include <topics/sensor_gyro_fft.h>
#include <topics/sensor_gyro_fifo.h>
#include <topics/sensor_hygrometer.h>
#include <topics/sensor_mag.h>
#include <topics/sensor_optical_flow.h>
#include <topics/sensor_preflight_mag.h>
#include <topics/sensor_selection.h>
#include <topics/sensor_uwb.h>
#include <topics/sensors_status.h>
#include <topics/sensors_status_imu.h>
#include <topics/system_power.h>
#include <topics/takeoff_status.h>
#include <topics/task_stack_info.h>
#include <topics/tecs_status.h>
#include <topics/telemetry_status.h>
#include <topics/tiltrotor_extra_controls.h>
#include <topics/timesync_status.h>
#include <topics/trajectory_bezier.h>
#include <topics/trajectory_setpoint.h>
#include <topics/trajectory_waypoint.h>
#include <topics/transponder_report.h>
#include <topics/tune_control.h>
#include <topics/uavcan_parameter_request.h>
#include <topics/uavcan_parameter_value.h>
#include <topics/ulog_stream.h>
#include <topics/ulog_stream_ack.h>
#include <topics/vehicle_acceleration.h>
#include <topics/vehicle_air_data.h>
#include <topics/vehicle_angular_acceleration_setpoint.h>
#include <topics/vehicle_angular_velocity.h>
#include <topics/vehicle_attitude.h>
#include <topics/vehicle_attitude_setpoint.h>
#include <topics/vehicle_command.h>
#include <topics/vehicle_command_ack.h>
#include <topics/vehicle_constraints.h>
#include <topics/vehicle_control_mode.h>
#include <topics/vehicle_global_position.h>
#include <topics/vehicle_imu.h>
#include <topics/vehicle_imu_status.h>
#include <topics/vehicle_land_detected.h>
#include <topics/vehicle_local_position.h>
#include <topics/vehicle_local_position_setpoint.h>
#include <topics/vehicle_magnetometer.h>
#include <topics/vehicle_odometry.h>
#include <topics/vehicle_optical_flow.h>
#include <topics/vehicle_optical_flow_vel.h>
#include <topics/vehicle_rates_setpoint.h>
#include <topics/vehicle_roi.h>
#include <topics/vehicle_status.h>
#include <topics/vehicle_thrust_setpoint.h>
#include <topics/vehicle_torque_setpoint.h>
#include <topics/vehicle_trajectory_bezier.h>
#include <topics/vehicle_trajectory_waypoint.h>
#include <topics/vtol_vehicle_status.h>
#include <topics/wind.h>
#include <topics/yaw_estimator_status.h>


const constexpr struct orb_metadata *const uorb_topics_list[ORB_TOPICS_COUNT] = {
	ORB_ID(action_request), 
	ORB_ID(actuator_armed), 
	ORB_ID(actuator_controls_status_0), 
	ORB_ID(actuator_controls_status_1), 
	ORB_ID(actuator_motors), 
	ORB_ID(actuator_outputs), 
	ORB_ID(actuator_outputs_debug), 
	ORB_ID(actuator_outputs_sim), 
	ORB_ID(actuator_servos), 
	ORB_ID(actuator_servos_trim), 
	ORB_ID(actuator_test), 
	ORB_ID(adc_report), 
	ORB_ID(airspeed), 
	ORB_ID(airspeed_validated), 
	ORB_ID(airspeed_wind), 
	ORB_ID(autotune_attitude_control_status), 
	ORB_ID(battery_status), 
	ORB_ID(button_event), 
	ORB_ID(camera_capture), 
	ORB_ID(camera_status), 
	ORB_ID(camera_trigger), 
	ORB_ID(cellular_status), 
	ORB_ID(collision_constraints), 
	ORB_ID(collision_report), 
	ORB_ID(control_allocator_status), 
	ORB_ID(cpuload), 
	ORB_ID(debug_array), 
	ORB_ID(debug_key_value), 
	ORB_ID(debug_value), 
	ORB_ID(debug_vect), 
	ORB_ID(differential_pressure), 
	ORB_ID(distance_sensor), 
	ORB_ID(ekf2_timestamps), 
	ORB_ID(esc_report), 
	ORB_ID(esc_status), 
	ORB_ID(estimator_aid_src_airspeed), 
	ORB_ID(estimator_aid_src_aux_vel), 
	ORB_ID(estimator_aid_src_baro_hgt), 
	ORB_ID(estimator_aid_src_ev_hgt), 
	ORB_ID(estimator_aid_src_ev_pos), 
	ORB_ID(estimator_aid_src_ev_vel), 
	ORB_ID(estimator_aid_src_ev_yaw), 
	ORB_ID(estimator_aid_src_fake_hgt), 
	ORB_ID(estimator_aid_src_fake_pos), 
	ORB_ID(estimator_aid_src_gnss_hgt), 
	ORB_ID(estimator_aid_src_gnss_pos), 
	ORB_ID(estimator_aid_src_gnss_vel), 
	ORB_ID(estimator_aid_src_gnss_yaw), 
	ORB_ID(estimator_aid_src_gravity), 
	ORB_ID(estimator_aid_src_mag), 
	ORB_ID(estimator_aid_src_mag_heading), 
	ORB_ID(estimator_aid_src_optical_flow), 
	ORB_ID(estimator_aid_src_rng_hgt), 
	ORB_ID(estimator_aid_src_sideslip), 
	ORB_ID(estimator_aid_src_terrain_optical_flow), 
	ORB_ID(estimator_attitude), 
	ORB_ID(estimator_baro_bias), 
	ORB_ID(estimator_bias3d), 
	ORB_ID(estimator_ev_pos_bias), 
	ORB_ID(estimator_event_flags), 
	ORB_ID(estimator_global_position), 
	ORB_ID(estimator_gnss_hgt_bias), 
	ORB_ID(estimator_gps_status), 
	ORB_ID(estimator_innovation_test_ratios), 
	ORB_ID(estimator_innovation_variances), 
	ORB_ID(estimator_innovations), 
	ORB_ID(estimator_local_position), 
	ORB_ID(estimator_odometry), 
	ORB_ID(estimator_optical_flow_vel), 
	ORB_ID(estimator_rng_hgt_bias), 
	ORB_ID(estimator_selector_status), 
	ORB_ID(estimator_sensor_bias), 
	ORB_ID(estimator_states), 
	ORB_ID(estimator_status), 
	ORB_ID(estimator_status_flags), 
	ORB_ID(estimator_wind), 
	ORB_ID(event), 
	ORB_ID(external_ins_attitude), 
	ORB_ID(external_ins_global_position), 
	ORB_ID(external_ins_local_position), 
	ORB_ID(failsafe_flags), 
	ORB_ID(failure_detector_status), 
	ORB_ID(flaps_setpoint), 
	ORB_ID(follow_target), 
	ORB_ID(follow_target_estimator), 
	ORB_ID(follow_target_status), 
	ORB_ID(fw_virtual_attitude_setpoint), 
	ORB_ID(generator_status), 
	ORB_ID(geofence_result), 
	ORB_ID(gimbal_controls), 
	ORB_ID(gimbal_device_attitude_status), 
	ORB_ID(gimbal_device_information), 
	ORB_ID(gimbal_device_set_attitude), 
	ORB_ID(gimbal_manager_information), 
	ORB_ID(gimbal_manager_set_attitude), 
	ORB_ID(gimbal_manager_set_manual_control), 
	ORB_ID(gimbal_manager_status), 
	ORB_ID(gimbal_v1_command), 
	ORB_ID(gps_dump), 
	ORB_ID(gps_inject_data), 
	ORB_ID(gripper), 
	ORB_ID(health_report), 
	ORB_ID(heater_status), 
	ORB_ID(home_position), 
	ORB_ID(hover_thrust_estimate), 
	ORB_ID(input_rc), 
	ORB_ID(internal_combustion_engine_status), 
	ORB_ID(iridiumsbd_status), 
	ORB_ID(irlock_report), 
	ORB_ID(landing_gear), 
	ORB_ID(landing_gear_wheel), 
	ORB_ID(landing_target_innovations), 
	ORB_ID(landing_target_pose), 
	ORB_ID(launch_detection_status), 
	ORB_ID(led_control), 
	ORB_ID(log_message), 
	ORB_ID(logger_status), 
	ORB_ID(mag_worker_data), 
	ORB_ID(magnetometer_bias_estimate), 
	ORB_ID(manual_control_input), 
	ORB_ID(manual_control_setpoint), 
	ORB_ID(manual_control_switches), 
	ORB_ID(mavlink_log), 
	ORB_ID(mavlink_tunnel), 
	ORB_ID(mc_virtual_attitude_setpoint), 
	ORB_ID(mission), 
	ORB_ID(mission_result), 
	ORB_ID(mode_completed), 
	ORB_ID(mount_orientation), 
	ORB_ID(navigator_mission_item), 
	ORB_ID(npfg_status), 
	ORB_ID(obstacle_distance), 
	ORB_ID(obstacle_distance_fused), 
	ORB_ID(offboard_control_mode), 
	ORB_ID(onboard_computer_status), 
	ORB_ID(orb_multitest), 
	ORB_ID(orb_test), 
	ORB_ID(orb_test_large), 
	ORB_ID(orb_test_medium), 
	ORB_ID(orb_test_medium_multi), 
	ORB_ID(orb_test_medium_queue), 
	ORB_ID(orb_test_medium_queue_poll), 
	ORB_ID(orb_test_medium_wrap_around), 
	ORB_ID(orbit_status), 
	ORB_ID(parameter_update), 
	ORB_ID(ping), 
	ORB_ID(position_controller_landing_status), 
	ORB_ID(position_controller_status), 
	ORB_ID(position_setpoint), 
	ORB_ID(position_setpoint_triplet), 
	ORB_ID(power_button_state), 
	ORB_ID(power_monitor), 
	ORB_ID(pps_capture), 
	ORB_ID(pwm_input), 
	ORB_ID(px4io_status), 
	ORB_ID(qshell_req), 
	ORB_ID(qshell_retval), 
	ORB_ID(radio_status), 
	ORB_ID(rate_ctrl_status), 
	ORB_ID(rc_channels), 
	ORB_ID(rc_parameter_map), 
	ORB_ID(rpm), 
	ORB_ID(rtl_time_estimate), 
	ORB_ID(safety_button), 
	ORB_ID(satellite_info), 
	ORB_ID(sensor_accel), 
	ORB_ID(sensor_accel_fifo), 
	ORB_ID(sensor_baro), 
	ORB_ID(sensor_combined), 
	ORB_ID(sensor_correction), 
	ORB_ID(sensor_gnss_relative), 
	ORB_ID(sensor_gps), 
	ORB_ID(sensor_gyro), 
	ORB_ID(sensor_gyro_fft), 
	ORB_ID(sensor_gyro_fifo), 
	ORB_ID(sensor_hygrometer), 
	ORB_ID(sensor_mag), 
	ORB_ID(sensor_optical_flow), 
	ORB_ID(sensor_preflight_mag), 
	ORB_ID(sensor_selection), 
	ORB_ID(sensor_uwb), 
	ORB_ID(sensors_status_baro), 
	ORB_ID(sensors_status_imu), 
	ORB_ID(sensors_status_mag), 
	ORB_ID(spoilers_setpoint), 
	ORB_ID(system_power), 
	ORB_ID(takeoff_status), 
	ORB_ID(task_stack_info), 
	ORB_ID(tecs_status), 
	ORB_ID(telemetry_status), 
	ORB_ID(tiltrotor_extra_controls), 
	ORB_ID(timesync_status), 
	ORB_ID(trajectory_bezier), 
	ORB_ID(trajectory_setpoint), 
	ORB_ID(trajectory_waypoint), 
	ORB_ID(transponder_report), 
	ORB_ID(tune_control), 
	ORB_ID(uavcan_parameter_request), 
	ORB_ID(uavcan_parameter_value), 
	ORB_ID(ulog_stream), 
	ORB_ID(ulog_stream_ack), 
	ORB_ID(vehicle_acceleration), 
	ORB_ID(vehicle_air_data), 
	ORB_ID(vehicle_angular_acceleration_setpoint), 
	ORB_ID(vehicle_angular_velocity), 
	ORB_ID(vehicle_angular_velocity_groundtruth), 
	ORB_ID(vehicle_attitude), 
	ORB_ID(vehicle_attitude_groundtruth), 
	ORB_ID(vehicle_attitude_setpoint), 
	ORB_ID(vehicle_command), 
	ORB_ID(vehicle_command_ack), 
	ORB_ID(vehicle_constraints), 
	ORB_ID(vehicle_control_mode), 
	ORB_ID(vehicle_global_position), 
	ORB_ID(vehicle_global_position_groundtruth), 
	ORB_ID(vehicle_gps_position), 
	ORB_ID(vehicle_imu), 
	ORB_ID(vehicle_imu_status), 
	ORB_ID(vehicle_land_detected), 
	ORB_ID(vehicle_local_position), 
	ORB_ID(vehicle_local_position_groundtruth), 
	ORB_ID(vehicle_local_position_setpoint), 
	ORB_ID(vehicle_magnetometer), 
	ORB_ID(vehicle_mocap_odometry), 
	ORB_ID(vehicle_odometry), 
	ORB_ID(vehicle_optical_flow), 
	ORB_ID(vehicle_optical_flow_vel), 
	ORB_ID(vehicle_rates_setpoint), 
	ORB_ID(vehicle_roi), 
	ORB_ID(vehicle_status), 
	ORB_ID(vehicle_thrust_setpoint), 
	ORB_ID(vehicle_thrust_setpoint_virtual_fw), 
	ORB_ID(vehicle_thrust_setpoint_virtual_mc), 
	ORB_ID(vehicle_torque_setpoint), 
	ORB_ID(vehicle_torque_setpoint_virtual_fw), 
	ORB_ID(vehicle_torque_setpoint_virtual_mc), 
	ORB_ID(vehicle_trajectory_bezier), 
	ORB_ID(vehicle_trajectory_waypoint), 
	ORB_ID(vehicle_trajectory_waypoint_desired), 
	ORB_ID(vehicle_visual_odometry), 
	ORB_ID(vtol_vehicle_status), 
	ORB_ID(wind), 
	ORB_ID(yaw_estimator_status), 

};

const struct orb_metadata *const *orb_get_topics()
{
	return uorb_topics_list;
}

const struct orb_metadata *get_orb_meta(ORB_ID id)
{
	if (id == ORB_ID::INVALID) {
		return nullptr;
	}

	return uorb_topics_list[static_cast<uint8_t>(id)];
}
