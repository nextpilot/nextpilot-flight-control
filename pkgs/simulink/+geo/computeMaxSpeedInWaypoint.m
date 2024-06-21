function y = computeMaxSpeedInWaypoint(alpha,accel,d)

tan_alpha = tan(alpha / 2);
y = sqrt(accel * d * tan_alpha);

end

