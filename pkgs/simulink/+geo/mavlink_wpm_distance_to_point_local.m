% Calculate distance in local frame (NED)
function [dist, dist_xy, dist_z] = mavlink_wpm_distance_to_point_local(x_now, y_now, z_now, x_next, y_next, z_next)

dx = x_now - x_next;
dy = y_now - y_next;
dz = z_now - z_next;

dist_xy = sqrt(dx * dx + dy * dy);
dist_z = abs(dz);
dist = sqrt(dx * dx + dy * dy + dz * dz);

end


