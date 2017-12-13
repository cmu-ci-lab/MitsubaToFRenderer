clc, clear, close all

addpath('../EllipsoidPlaneIntersection');

%% Light source at (1, 0, 0) and camera at (0, 0, 0)

Time_min = 2;
Time_max = 6;
Time_res =1e-2;

Time = Time_min:Time_res:Time_max;

packets = 1;
N = 1e6; % Total rays = packets*N;

tic
% PR = PhotonEllipticRenderer(Time_min, Time_max, Time_res, packets, N);
SR = SurfaceEllipticRenderer(Time_min, Time_max, Time_res, packets, N);
% CR = CircleRenderer(Time_min, Time_max, Time_res, packets, N);
ERT= EllipticRenderer_timeRandomized(Time_min, Time_max, Time_res, packets, N);
% CRR= CircleRenderer_radiusRandomized(Time_min, Time_max, Time_res, packets, N);

% figure, plot(Time, PR), title('Photon Renderer');
figure, plot(Time, SR), title('Surface Renderer');
figure, plot(Time, ERT), title('Elliptic Renderer');

% figure, plot(PR./SR), title('PR/SR');
figure, plot(ERT./SR), title('ERT/SR');
% figure, plot(CRR./SR), title('CRR/SR');
toc