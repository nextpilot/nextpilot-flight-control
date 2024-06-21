function y = computeMaxSpeedFromDistance(jerk,accel,braking_distance,final_speed)

b = 4 * accel^2 / jerk;
c = -2 * accel * braking_distance - final_speed^2;
max_speed = 0.5 * (-b + sqrt(b^2 - 4 * c));

y = max(max_speed, final_speed);

end

