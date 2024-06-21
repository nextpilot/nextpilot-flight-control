% /**
% * Get a new waypoint based on heading and distance from current position
% *
% * @param heading the heading to fly to
% * @param distance the distance of the generated waypoint
% * @param waypoint_prev the waypoint at the current position
% * @param waypoint_next the waypoint in the heading direction
% */

function [waypoint_prev, waypoint_next] = get_waypoint_heading_distance(heading, waypoint_prev, waypoint_next, plane_pos, holding_alt, flag_init)

    HDG_HOLD_SET_BACK_DIST = 100;  % distance by which previous waypoint is set behind the plane
    HDG_HOLD_REACHED_DIST  = 1000; % distance (plane to waypoint in front) at which waypoints are reset in heading hold mode
    HDG_HOLD_DIST_NEXT     = 3000; % initial distance of waypoint in front of plane in heading hold mode
    
    temp_prev = waypoint_prev;
    temp_next = waypoint_next;

    if (flag_init)
        % previous waypoint: HDG_HOLD_SET_BACK_DIST meters behind us
        [temp_prev(1), temp_prev(2)] = waypoint_from_heading_and_distance(plane_pos(1), plane_pos(2), heading + deg2rad(180), HDG_HOLD_SET_BACK_DIST);
        % next waypoint: HDG_HOLD_DIST_NEXT meters in front of us
        [temp_next(1), temp_next(2)] = waypoint_from_heading_and_distance(plane_pos(1), plane_pos(2), heading, HDG_HOLD_DIST_NEXT);
    else
        % use the existing flight path from prev to next
        % previous waypoint: shifted HDG_HOLD_REACHED_DIST + HDG_HOLD_SET_BACK_DIST
        [temp_prev(1), temp_prev(2)] = create_waypoint_from_line_and_dist(waypoint_next(1), waypoint_next(2), waypoint_prev(1), waypoint_prev(2),  (HDG_HOLD_REACHED_DIST + HDG_HOLD_SET_BACK_DIST));
        % next waypoint: shifted -(HDG_HOLD_DIST_NEXT + HDG_HOLD_REACHED_DIST)
        [temp_next(1), temp_next(2)] = create_waypoint_from_line_and_dist(waypoint_next(1), waypoint_next(2), waypoint_prev(1), waypoint_prev(2), -(HDG_HOLD_REACHED_DIST + HDG_HOLD_DIST_NEXT));
    end

    % waypoint_prev[lat, lon, alt]
    waypoint_prev = temp_prev;
    waypoint_prev(3) = holding_alt;
    
    waypoint_next = temp_next;
    waypoint_next(3) = holding_alt;

end

