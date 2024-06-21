function y = computeStartXYSpeedFromWaypoints(start_position,target_position,next_target,exit_speed,z_accept,xy_accept,max_acc_xy,max_jerk,max_speed_xy,max_acc_xy_radius_scale)

distance_target_next = norm([target_position(1) - next_target(1), target_position(2) - next_target(2)]);
target_next_different = (distance_target_next > 0.001);
waypoint_overlap = (distance_target_next < xy_accept);
has_reached_altitude = (abs(target_position(3) - start_position(3)) < z_accept);
altitude_stays_same = (abs(next_target(3) - target_position(3)) < z_accept);

speed_at_target = single(0);

if target_next_different && ~waypoint_overlap && has_reached_altitude && altitude_stays_same
    a = [target_position(1) - start_position(1), target_position(2) - start_position(2)];
    b = [target_position(1) - next_target(1), target_position(2) - next_target(2)];
    alpha = acos(dot(vector2_unit_or_zero(a), vector2_unit_or_zero(b)));
    safe_alpha = constrain(alpha, single(0), (pi - eps));
    accel_tmp = max_acc_xy_radius_scale * max_acc_xy;
    max_speed_in_turn = computeMaxSpeedInWaypoint(safe_alpha, accel_tmp, xy_accept);
    speed_at_target = min([max_speed_in_turn, exit_speed, max_speed_xy]);
end

start_to_target = norm([start_position(1) - target_position(1), start_position(2) - target_position(2)]);
max_speed = computeMaxSpeedFromDistance(max_jerk, max_acc_xy, start_to_target, speed_at_target);

y = min(max_speed_xy, max_speed);

end

