clc, clear,% close all

addpath('../');
%% Ellipse focal points and the tau value
% p1 = [1 1 0];
% p2 = [0 .2 0];
% tau = 2;

% p1 = [ 1 0 0];
% p2 = [-1 0 0];
% tau= 5.42444;

p1 = [198.542435 548.799988 310.528351];
p2 = [277.928436 272.121521 291.990540];
tau= 1.1*norm(p1-p2);
%% Polygon
Polygon = [ 250+50       250  250;
            400       400+50  400-248.80;
            200+259.2 200  200+50;
            1  1  1;];

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
PolygonEllipse =Tranform3D2Ellipsoid * Polygon;

%% Compute center of the ellipse 
Tranform3D2Sphere  = Scaling*Tranform3D2Ellipsoid;
PolygonSphere =Tranform3D2Sphere * Polygon;
N = cross(PolygonSphere(1:3,2)-PolygonSphere(1:3,1), PolygonSphere(1:3,3)-PolygonSphere(1:3,1));
N = N./norm(N);
CSphere = dot(PolygonSphere(1:3,1),N)*N;
CEllipse = inv(Scaling)*[CSphere;1];
O = CEllipse(1:3);

%% Compute the angle of ellipse with T
T = (PolygonEllipse(1:3,2)-PolygonEllipse(1:3,1));
T = T./norm(T);
n = cross(T, PolygonEllipse(1:3,3)-PolygonEllipse(1:3,1));
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

m1 = sqrt(2 * (1-OOD)/(TTD+UUD-sqrt(4*TUD^2+(TTD-UUD)^2)));
m2 = sqrt(2 * (1-OOD)/(TTD+UUD+sqrt(4*TUD^2+(TTD-UUD)^2)));

%% Plotting everything
thetas = 0:.001:2*pi;

ellipticPoints = [m1*cos(thetas);m2*sin(thetas)];
ellipticPoints(3,:) = 0;
ellipticPoints(4,:) = 1;

ReprojectedIntersectionPoints = inv(Tranform3D2Ellipsoid)*inv(TranformEllipsoid2Ellipse)*ellipticPoints;

figure,

NPoints = 100;
[X,Y,Z] = sphere(NPoints);
% Transform to make it the ellipse we have
Points = [X(:)';Y(:)';Z(:)'];
Points(end+1,:) = 1;
Points = inv(Tranform3D2Sphere) * Points;
X = reshape(Points(1,:), NPoints+1, NPoints+1);
Y = reshape(Points(2,:), NPoints+1, NPoints+1);
Z = reshape(Points(3,:), NPoints+1, NPoints+1);


plot3(ReprojectedIntersectionPoints(1,:),ReprojectedIntersectionPoints(2,:),ReprojectedIntersectionPoints(3,:),'r+');hold on
% xlim([min(X(:)),max(X(:))]);
% ylim([min(Y(:)),max(Y(:))]);
% zlim([min(Z(:)),max(Z(:))]);
patch(Polygon(1,:),Polygon(2,:),Polygon(3,:),'b');hold on;
% xlim([min(ReprojectedIntersectionPoints(1,:)),max(ReprojectedIntersectionPoints(1,:))+1]);
% ylim([min(ReprojectedIntersectionPoints(2,:)),max(ReprojectedIntersectionPoints(2,:))+1]);
% zlim([min(ReprojectedIntersectionPoints(3,:)),max(ReprojectedIntersectionPoints(3,:))+1]);
% pause(0.5)
% pause(0.5)

plot3(p1(1), p1(2), p1(3),'bo');
plot3(p2(1), p2(2), p2(3),'bo');
pause(0.5)
mesh(X, Y, Z);hold on;

xlabel('x');
ylabel('y');
zlabel('z');

axis square


