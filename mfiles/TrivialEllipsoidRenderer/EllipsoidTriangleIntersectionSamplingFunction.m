function [P, measure, intersects] = EllipsoidTriangleIntersectionSamplingFunction(p1, p2, tau, Polygon)

intersects = false;
P = [0,0,0,1];
measure = 0;

%% Compute the transform to center the ellipsoid at the origin and axis aligned with elliposoids axis
a = tau/2;
C = (p1 + p2)/2;
b = sqrt(a^2-norm(C-p1)^2);

if(a^2<norm(C-p1)^2)
    disp('impossible ellipse');
    return;
end

D = p2-p1;

Translation = makehgtform('translate',-C);
Rotation    = Vectors2RotationMatrix(D, [1 0 0]);
Scaling     = makehgtform('scale',[1/a 1/b 1/b]);

Tranform3D2Ellipsoid = Rotation*Translation;
PolygonEllipsoid =Tranform3D2Ellipsoid * Polygon;

%% Compute center of the ellipse 
Tranform3D2Sphere  = Scaling*Tranform3D2Ellipsoid;
PolygonSphere =Tranform3D2Sphere * Polygon;
N = cross(PolygonSphere(1:3,2)-PolygonSphere(1:3,1), PolygonSphere(1:3,3)-PolygonSphere(1:3,1));
N = N./norm(N);
CSphere = dot(PolygonSphere(1:3,1),N)*N;
CEllipse = inv(Scaling)*[CSphere;1];
O = CEllipse(1:3);

if(norm(CSphere) > 1) % ellipsoid does not intersect plane
    return;
end

%% Compute the angle of ellipse with T
T = (PolygonEllipsoid(1:3,2)-PolygonEllipsoid(1:3,1));
T = T./norm(T);
n = cross(T, PolygonEllipsoid(1:3,3)-PolygonEllipsoid(1:3,1));
n = n./norm(n);
U = cross(n, T);

TTD = weightIP(T, T, a, b, b);
TUD = weightIP(T, U, a, b, b);
UUD = weightIP(U, U, a, b, b);
OOD = weightIP(O, O, a, b, b);

% theta = .5 *atan2((UUD-TTD),2*TUD)-pi/4;
% theta = .5 *atan2((UUD-TTD),2*TUD)-pi/4
theta = .5*atan2(2*TUD,(UUD-TTD));

NewX =  T*cos(theta) - U*sin(theta);
NewY =  T*sin(theta) + U*cos(theta);

%% Compute transform to shift axis of ellipsoid to ellipse
TranformEllipsoid2Ellipse = inv([NewX NewY n]);
% TranformEllipsoid2Ellipse = inv([T U n]);
TranformEllipsoid2Ellipse(4,4) = 1;
TranformEllipsoid2Ellipse = TranformEllipsoid2Ellipse * makehgtform('translate',-O);

%% Compute major axis and minor axis
det = sqrt(4*TUD^2+(TTD-UUD)^2);
m1 = sqrt(2 * (1-OOD)/(TTD+UUD-det));
m2 = sqrt(2 * (1-OOD)/(TTD+UUD+det));
k  = sqrt(1-m2*m2/(m1*m1));

%% Compute transform to scale ellipse to circle
TranformEllipse2Circle = makehgtform('scale',[1 m1/m2 1]);
PolygonCircle = TranformEllipse2Circle * TranformEllipsoid2Ellipse * PolygonEllipsoid;

%% Circle Triangle Intersection to get all the angles
[theta_min, theta_max, indices] = circlePolygonIntersectionAngles(PolygonCircle(1:2,:), m1);

if(indices == 0)
%     disp('Ellipsoid and triangle does not intersect');
    return
end

%% Sample an angle in the theta_range
cumsum_theta = cumsum(theta_max - theta_min);
theta_range  = cumsum_theta(end);
theta_s       = rand * theta_range;
if(theta_s <= cumsum(1))
    theta = theta_min(1) + theta_s;
else
    for i=2:indices
        if(theta_s <= cumsum_theta(i))
            theta = theta_min(i) + theta_s - cumsum(i-1);
        end
    end
end

%% Compute the point
ellipticPoints = [m1*cos(theta);m2*sin(theta)];
ellipticPoints(3,:) = 0;
ellipticPoints(4,:) = 1;
% 
P = inv(Tranform3D2Ellipsoid)*inv(TranformEllipsoid2Ellipse)*ellipticPoints;


TTE = WeightedIP(T, T, -(1/a^3), -(a/b^4), -(a/b^4));
TUE = WeightedIP(T, U, -(1/a^3), -(a/b^4), -(a/b^4));
UUE = WeightedIP(U, U, -(1/a^3), -(a/b^4), -(a/b^4));
OOE = WeightedIP(O, O, -(1/a^3), -(a/b^4), -(a/b^4));
DR = (TTD+UUD-det);
dDR = TTE + UUE - (1/det)*(4*TUD*TUE + (TTE-UUE)*(TTD-UUD));

measure = (-DR*OOE-(1-OOD)*dDR)/(DR*DR)*sqrt(1-k*k)*theta_range;

intersects = true;
