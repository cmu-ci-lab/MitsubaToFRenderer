function [theta, x, y] = circleLineIntersection(point1, point2, r)

x1 = point1(1);
y1 = point1(2);
x2 = point2(1);
y2 = point2(2);

dx = x1 - x2;
dy = y1 - y2;
a = dx^2 + dy^2;
b = 2*(dx*x2+dy*y2);
c = x2^2+y2^2-r^2;

det = sqrt(b*b-4*a*c);

alpha = (-b+det)/(2*a);

x = 0;
y = eps;

if(alpha > 0 && alpha < 1)
    x=alpha*x1+(1-alpha)*x2;
    y=alpha*y1+(1-alpha)*y2;
else
    alpha = (-b-det)/(2*a);
    if(alpha > 0 && alpha < 1)
        x=alpha*x1+(1-alpha)*x2;
        y=alpha*y1+(1-alpha)*y2;
    else
        disp('circleLineIntersection Code shouldn''t be called');
    end
end

theta = atan2(y, x);
