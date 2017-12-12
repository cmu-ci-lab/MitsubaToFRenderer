clc, clear, close all
% 
% points = [-.375425 .267701   .292842;
%           -.53004  -.557264  .079370];

% 
for i=1:100
% for i=60:70
% for i=[4 98]
% for i=[96]
%     close all;
    rng(i);
    points = 2*rand(2,3)-1;

    norm_p = sqrt(sum(points.^2));
    r =2*rand;
%     r = norm_p(2);
%     points(:,3) = norm_p(2)/norm_p(3)*points(:,3);
%     points(:,1) = norm_p(2)/norm_p(1)*points(:,1);
    [thetamin, thetamax, indices] = circlePolygonIntersectionAngles(points, r);

    CircAngle = [];
    for j=1:indices
        CircAngle = [CircAngle thetamin(j):1e-3:thetamax(j)];
    end

    t = 0:0.01:2*pi;
    figure(1);
    plot(r*cos(t),r*sin(t),'b'); hold on
    plot(r*cos(CircAngle),r*sin(CircAngle),'r.'); hold on

    axis 'equal'

    plot(r*cos(CircAngle),r*sin(CircAngle),'r.'); hold on

    axis 'equal'

    points(:,end+1) = points(:, 1); 
    plot(points(1, :), points(2, :), 'k'); hold off
    xlim([-2 2]);
    ylim([-2 2]);
    
    title(strcat('Index:',num2str(i)));
    pause(1)
    
end

%% rough test
% Points = [-94.3340431, 119.190132;
%          58.4258347,  -119.190308;
%          100.976234, 76.5666733]';
%            
% m1 = 263.074615;
% [thetamin, thetamax, indices] = circlePolygonIntersectionAngles(Points, m1);
% Points = [-174.649 39.8256;
%          -49.9041,  -174.457;
%          106.425, -3.46693]';
%            
% m1 = 106.481;
% [thetamin, thetamax, indices] = circlePolygonIntersectionAngles(Points, m1);
% 
% Points = [-95.2955, -130.394;
%          92.0251,  -59.3928;
%          21.9467, 130.394]';
%            
% m1 = 161.505;
% [thetamin, thetamax, indices] = circlePolygonIntersectionAngles(Points, m1);

%% rough test plots
% CircAngle = [];
% for j=1:indices
%     CircAngle = [CircAngle thetamin(j):1e-3:thetamax(j)];
% end
% 
% t = 0:0.01:2*pi;
% figure(1);
% plot(r*cos(t),r*sin(t),'b'); hold on
% plot(r*cos(CircAngle),r*sin(CircAngle),'r.'); hold on
% 
% axis 'equal'
% 
% points(:,end+1) = points(:, 1); 
% plot(points(1, :), points(2, :), 'k'); hold off
% xlim([-2 2]);
% ylim([-2 2]);
