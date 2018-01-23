clc, clear, close all

addpath('../EllipsoidPlaneIntersection');


sourceIntensity = 1e7;


% Time_min = 0;
% Time_max = 1000;
% Time_res = 20;
bias = 2*(500-340.79998779296875);
Time_min = 560 - bias;
Time_max = 581 - bias;
Time_res = 20;

Time = (Time_min:Time_res:Time_max);


x_min_plane =  0;
x_max_plane =  200;
y_min_plane =  0;
y_max_plane =  200;
z_plane = -440.79998779296875;


packets = 1;
N = 4096*2^4; % Total rays = packets*N;


p1 = [-4.9999923706054688 -5                 -340.79998779296875];
p2 = [-1.2687225341796875 -0.883087158203125 -340.79998779296875];

% sourceIntensity = sourceIntensity/(p1(3)^2);

tic
PR = PhotonEllipticRenderer(x_min_plane, x_max_plane, y_min_plane, y_max_plane, z_plane, p1, p2, Time_min, Time_max, Time_res, packets, N);
SR = SurfaceEllipticRenderer(x_min_plane, x_max_plane, y_min_plane, y_max_plane, z_plane, p1, p2, Time_min, Time_max, Time_res, packets, N);
ERT= EllipticRenderer_timeRandomized(x_min_plane, x_max_plane, y_min_plane, y_max_plane, z_plane, p1, p2, Time_min, Time_max, Time_res, packets, N);

PR  = sourceIntensity *PR;
SR  = sourceIntensity *SR;
ERT = sourceIntensity *ERT;


figure, plot(Time, PR), title('Photon Renderer');
figure, plot(Time, SR), title('Surface Renderer');
figure, plot(Time, ERT), title('Elliptic Renderer');

figure, plot(Time,PR./SR), title('PR/SR');
figure, plot(Time,ERT./SR), title('ERT/SR');
% figure, plot(Time,CRR./SR), title('CRR/SR');
toc
% save('EllipicRenderingResults_1e9.mat')