function [result, lat, lon] = map_projection_reproject(ref, x, y)

CONSTANTS_RADIUS_OF_EARTH = 6371000;

x_rad = x / CONSTANTS_RADIUS_OF_EARTH;
y_rad = y / CONSTANTS_RADIUS_OF_EARTH;
c = sqrt(x_rad * x_rad + y_rad * y_rad);

if (abs(c) > 0)
    sin_c = sin(c);
    cos_c = cos(c);
    lat_rad = asin(cos_c * ref.sin_lat + (x_rad * sin_c * ref.cos_lat) / c);
    lon_rad = (ref.lon_rad + atan2(y_rad * sin_c, c * ref.cos_lat * cos_c - x_rad * ref.sin_lat * sin_c));
    lat = double(rad2deg(lat_rad));
    lon = double(rad2deg(lon_rad));
else
    lat = rad2deg(ref.lat_rad);
    lon = rad2deg(ref.lon_rad);
end

result = false;

end


