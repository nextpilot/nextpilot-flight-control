function [v_n,v_e] = get_vector_to_next_waypoint(lat_now,lon_now,lat_next,lon_next,earth_radius)

lat_now_rad = deg2rad(lat_now);
lat_next_rad = deg2rad(lat_next);
d_lon = deg2rad(lon_next) - deg2rad(lon_now);

v_n = single(earth_radius * (cos(lat_now_rad) * sin(lat_next_rad) - sin(lat_now_rad) * cos(lat_next_rad) * cos(d_lon)));
v_e = single(earth_radius * sin(d_lon) * cos(lat_next_rad));

end

