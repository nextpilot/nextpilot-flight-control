function y = computeXYSpeedFromWaypoints(waypoints,z_accept,xy_accept,max_acc_xy,max_jerk,max_speed_xy,max_acc_xy_radius_scale)

max_speed = single(0);

for j = 1:1:length(waypoints)-1
    i = length(waypoints) -j;
    max_speed = computeStartXYSpeedFromWaypoints(waypoints(:,i), waypoints(:,i+1), waypoints(:,min(i+2,length(waypoints))),...
                                                max_speed,z_accept,xy_accept,max_acc_xy,max_jerk,max_speed_xy,max_acc_xy_radius_scale);
end

y = max_speed;

end

