% Creates a new waypoint C on the line of two given waypoints (A, B) at certain distance
% from waypoint A
% 
% @param lat_A waypoint A latitude in degrees (47.1234567°, not 471234567°)
% @param lon_A waypoint A longitude in degrees (8.1234567°, not 81234567°)
% @param lat_B waypoint B latitude in degrees (47.1234567°, not 471234567°)
% @param lon_B waypoint B longitude in degrees (8.1234567°, not 81234567°)
% @param dist distance of target waypoint from waypoint A in meters (can be negative)
% @param lat_target latitude of target waypoint C in degrees (47.1234567°, not 471234567°)
% @param lon_target longitude of target waypoint C in degrees (47.1234567°, not 471234567°)

function [lat_target, lon_target] = create_waypoint_from_line_and_dist(lat_A, lon_A, lat_B, lon_B, dist)

if (abs(dist) < single(eps))
    lat_target = lat_A;
    lon_target = lon_A;
else
    heading = get_bearing_to_next_waypoint(lat_A, lon_A, lat_B, lon_B);
    [lat_target, lon_target] = waypoint_from_heading_and_distance(lat_A, lon_A, heading, dist);
end

end


