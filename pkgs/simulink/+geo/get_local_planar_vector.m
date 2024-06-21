% /**
% * Convert a 2D vector from WGS84 to planar coordinates.
% *
% * This converts from latitude and longitude to planar
% * coordinates with (0,0) being at the position of ref and
% * returns a vector in meters towards wp.
% *
% * @param ref The reference position in WGS84 coordinates
% * @param wp The point to convert to into the local coordinates, in WGS84 coordinates
% * @return The vector in meters pointing from the reference position to the coordinates
% */

function [vector] = get_local_planar_vector(origin, target)

CONSTANTS_RADIUS_OF_EARTH = 6371000;

% this is an approximation for small angles, proposed by [2]
x_angle = deg2rad(target(1) - origin(1));
y_angle = deg2rad(target(2) - origin(2));
x_origin_cos = cos(deg2rad(origin(1)));

vector = [single(x_angle * CONSTANTS_RADIUS_OF_EARTH), single(y_angle * x_origin_cos * CONSTANTS_RADIUS_OF_EARTH)];

end

