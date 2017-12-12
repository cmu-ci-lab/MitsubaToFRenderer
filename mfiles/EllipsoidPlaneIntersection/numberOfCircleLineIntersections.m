function count = numberOfCircleLineIntersections(point1, point2, r)

x1 = point1(1);
y1 = point1(2);
x2 = point2(1);
y2 = point2(2);

dx = x1 - x2;
dy = y1 - y2;
a = dx^2 + dy^2;
b = 2*(dx*x2+dy*y2);
c = x2^2+y2^2-r^2;

alpha1 = (-b+sqrt(b*b-4*a*c))/(2*a);
alpha2 = (-b-sqrt(b*b-4*a*c))/(2*a);
    
count = 0;
if(alpha1 >= 0 && alpha1 <= 1)
    count = count + 1;
end
if(alpha2 >= 0 && alpha2 <= 1)
    count = count + 1;
end