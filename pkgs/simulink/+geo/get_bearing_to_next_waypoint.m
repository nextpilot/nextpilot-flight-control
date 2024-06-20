% Returns the bearing to the next waypoint in radians.
% 
% @param lat_now current position in degrees (47.1234567°, not 471234567°)
% @param lon_now current position in degrees (8.1234567°, not 81234567°)
% @param lat_next next waypoint position in degrees (47.1234567°, not 471234567°)
% @param lon_next next waypoint position in degrees (8.1234567°, not 81234567°)

function bearing = get_bearing_to_next_waypoint(lat_now, lon_now, lat_next, lon_next)

lat_now_rad  = deg2rad(lat_now);
lat_next_rad = deg2rad(lat_next);

cos_lat_next = cos(lat_next_rad);
d_lon = deg2rad(lon_next - lon_now);

% conscious mix of double and float trig function to maximize speed and efficiency
y = (sin(d_lon) * cos_lat_next);
x = (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos_lat_next * cos(d_lon));

bearing = single(wrap(atan2(y, x), -pi, pi));

end

