function [lat_target, lon_target] = waypoint_from_heading_and_distance(lat_start, lon_start, bearing, dist)

    CONSTANTS_RADIUS_OF_EARTH = 6371000;

    radius_ratio = dist / CONSTANTS_RADIUS_OF_EARTH;

    lat_start_rad = deg2rad(lat_start);
    lon_start_rad = deg2rad(lon_start);

    lat_target = asin(sin(lat_start_rad) * cos(radius_ratio) + cos(lat_start_rad) * sin(radius_ratio) * cos(bearing));
    lon_target = lon_start_rad + atan2(sin(bearing) * sin(radius_ratio) * cos(lat_start_rad), cos(radius_ratio) - sin(lat_start_rad) * sin(lat_target));

    lat_target = double(rad2deg(lat_target));
    lon_target = double(rad2deg(lon_target));

end