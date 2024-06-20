function ref = map_projection_init(lat_0, lon_0)

% lat_0, lon_0 are expected to be in correct format: -> 47.1234567 and not 471234567
ref = map_projection_init_timestamped(lat_0, lon_0, uint64(0));

end


