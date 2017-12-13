function s = WeightedIP( vec1, vec2, a, b, c )
    s = sum(vec1.*vec2.*[a;b;c]);
end

