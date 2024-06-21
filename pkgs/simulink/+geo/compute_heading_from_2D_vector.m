function y = compute_heading_from_2D_vector(u, v)

y = single(u);

if isfinite(norm(v)) && (norm(v) > 0.001)
    v = v / norm(v);
    y = sign(v(2)) * constrain(acos(v(1)), -single(pi), single(pi));
end

end

