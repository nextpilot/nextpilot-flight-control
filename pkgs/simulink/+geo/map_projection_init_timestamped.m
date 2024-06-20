function ref = map_projection_init_timestamped(lat_0, lon_0, timestamp)

ref.timestamp = uint64(0);
ref.lat_rad = deg2rad(lat_0);
ref.lon_rad = deg2rad(lon_0);
ref.sin_lat = sin(deg2rad(lat_0));
ref.cos_lat = cos(deg2rad(lat_0));
ref.init_done = true;

end

