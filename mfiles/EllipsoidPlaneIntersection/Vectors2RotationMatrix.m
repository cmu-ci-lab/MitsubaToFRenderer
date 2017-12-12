function R = Vectors2RotationMatrix( a,b )
%VECTORS2ROTATIONMATRIX Summary of this function goes here
%   Rotates vector a to vector b

R = eye(4);

if(norm(a) == 0 || norm(b) == 0)
    return;
end

a = a/norm(a);
b = b/norm(b);

v = cross(a,b);
s = norm(v);
if(s == 0)
    return;
end

c = dot(a,b);
vX=[0 -v(3) v(2);v(3) 0 -v(1);-v(2) v(1) 0];

R = eye(3) + vX + vX^2 * (1-c)/s^2;
R(end+1,end+1)=1;


end

