function [P, measure, intersects] = EllipsoidTriangleIntersectionSamplingFunction(p1, p2, tau, Polygon)

intersects = false;
P = [0,0,0,1];
measure = 0;
%% Compute the transform to center the ellipsoid at the origin and axis aligned with elliposoids axis
a = tau/2;
C = (p1 + p2)/2;
b = sqrt(a^2-norm(C-p1)^2);

if(a^2<norm(C-p1)^2)
%     disp('impossible ellipse');
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


% TOD = weightIP(T, O, a, b, b);
% UOD = weightIP(U, O, a, b, b);

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
Delta = sqrt(4*TUD^2+(TTD-UUD)^2);
DR_1 = (TTD+UUD-Delta);
DR_2 = (TTD+UUD+Delta);
m1 = sqrt(2 * (1-OOD)/DR_1);
m2 = sqrt(2 * (1-OOD)/DR_2);
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
theta_s      = rand * theta_range;
if(theta_s <= cumsum_theta(1))
    theta = theta_min(1) + theta_s;
else
    for i=2:indices
        if(theta_s <= cumsum_theta(i))
            theta = theta_min(i) + theta_s - cumsum_theta(i-1);
        end
    end
end

%% Compute the point
ellipticPoints = [m1*cos(theta);m2*sin(theta)];
ellipticPoints(3,:) = 0;
ellipticPoints(4,:) = 1;
% 
P = inv(Tranform3D2Ellipsoid)*inv(TranformEllipsoid2Ellipse)*ellipticPoints;

% if( abs(norm(P(1:3) - p1') + norm(P(1:3) - p2') -tau) > 1e-7)
%     disp('Error in the generation of P')
% end


TTE = WeightedIP(T, T, -(1/a^3), -(a/b^4), -(a/b^4));
TUE = WeightedIP(T, U, -(1/a^3), -(a/b^4), -(a/b^4));
UUE = WeightedIP(U, U, -(1/a^3), -(a/b^4), -(a/b^4));
OOE = WeightedIP(O, O, -(1/a^3), -(a/b^4), -(a/b^4));
dDelta = (1/Delta)*(4*TUD*TUE + (TTE-UUE)*(TTD-UUD));
if(Delta < 1e-7)
    dDR_1 = TTE + UUE;
    dDR_2 = TTE + UUE;
else
    dDR_1 = TTE + UUE - dDelta;
    dDR_2 = TTE + UUE + dDelta;
end

%Calculate dO
dOM = ([T(1)/(a*a) T(2)/(b*b) T(3)/(b*b); ...
        U(1)/(a*a) U(2)/(b*b) U(3)/(b*b); ...
           n(1)       n(2)       n(3); ...
    ]);
dOV = tau*0.5*[T(1)*O(1)/a^4+T(2)*O(2)/b^4+T(3)*O(3)/b^4; ...
               U(1)*O(1)/a^4+U(2)*O(2)/b^4+U(3)*O(3)/b^4; ...
               0; ...
              ];
dO = dOM\dOV;

OdOD = weightIP(O, dO, a, b, b);
NR   = (1-OOD);
dNR  = -OOE - 2*OdOD;

msnthetadtheta = (Delta*(UUE-TTE)-dDelta*(UUD-TTD))/Delta^2;
cnthetadtheta  = 2 * (Delta*TUE-dDelta*TUD)/Delta^2;
dTN  = T*msnthetadtheta - U*cnthetadtheta;
dUN  = T*cnthetadtheta  + U*msnthetadtheta ;

dAxis = O'*(dTN*m2*cos(theta) + dUN*m1*sin(theta));

%% Jacobian checks:
% addpath('../CheckGrads');
% e = 10^(-6);
% specialparams = [T(1), T(2), T(3), U(1), U(2), U(3), n(1), n(2), n(3), PolygonEllipsoid(1,1), PolygonEllipsoid(2,1), PolygonEllipsoid(3,1), norm(C-p1)];
% [dt, dh, dv] = checkgrad('TDerivationCheckFunction', tau, e, specialparams);
% if(dt > 1e-5)
%     disp(strcat('Error in T derivation, d:', num2str(dt), ' dh:', num2str(dh), ' dv:', num2str(dv)));
% end
% [du, dh, dv] = checkgrad('UDerivationCheckFunction', tau, e, specialparams);
% if(du > 1e-5)
%     disp(strcat('Error in U derivation, d:', num2str(du), ' dh:', num2str(dh), ' dv:', num2str(dv)));
% end


measure = ( (DR_1*dNR-NR*dDR_1)/(DR_1*DR_1)*m2/m1*cos(theta)^2 + (DR_2*dNR-NR*dDR_2)/(DR_2*DR_2)*m1/m2*sin(theta)^2 + (m2*cos(theta)*NewX' + m1*sin(theta)*NewY')*dO + dAxis)*theta_range;

% addpath('../autodiffGenerators');
% specialparams = [T' U' n' PolygonEllipsoid(1:3,1)' PolygonEllipsoid(1:3,2)' PolygonEllipsoid(1:3,3)' norm(C-p1)];
% Ellipseparams = [tau; theta];
% measure = ComputeJacobianMeasure(Ellipseparams, specialparams) * theta_range;
% measure = (1/ComputeJacobianMeasure(Ellipseparams, specialparams)) * theta_range;
% measure = measure*area(PolygonEllipsoid(1:3,1)',PolygonEllipsoid(1:3,2),PolygonEllipsoid(1:3,3));

% if( abs(measure-measure_precompute) > 1e-4)
%     disp(strcat('Error in measure:', num2str(measure_precompute), ' vs ', num2str(measure)));  
% end

intersects = true;
end
% 
% function value = area(C1, C2, C3)
%     cr = cross(C1-C2, C1-C3);
%     value = 1/2*sqrt(cr(1)^2 + cr(2)^2 + cr(3)^2);
% end

