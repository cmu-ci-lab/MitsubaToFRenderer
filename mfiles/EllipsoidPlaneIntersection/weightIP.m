function s = weightIP( vec1, vec2, a, b, c )
    s = sum(vec1.*vec2.*[1/(a*a);1/(b*b);1/(c*c)]);
end

