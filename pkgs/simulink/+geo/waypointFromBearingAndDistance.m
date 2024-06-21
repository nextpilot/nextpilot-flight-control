function [test_point_lat,test_point_lon] = waypointFromBearingAndDistance(current_pos_lat,current_pos_lon,test_point_bearing,test_point_distance,earth_radius)

if test_point_distance < 0
    test_point_distance = - test_point_distance;
    test_point_bearing = wrap(test_point_bearing + pi, single(0), single(pi));
end

[test_point_lat,test_point_lon] = waypoint_from_heading_and_distance(current_pos_lat, current_pos_lon, test_point_bearing, test_point_distance, earth_radius);

end

