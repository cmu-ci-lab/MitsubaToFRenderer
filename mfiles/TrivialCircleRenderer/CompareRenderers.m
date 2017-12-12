clc, clear, close all

Time_min = 3;
Time_max = 6;
Time_res =1e-4;

Time = Time_min:Time_res:Time_max;

packets = 1;
N = 1e6; % Total rays = packets*N;

tic
% PR = PhotonRenderer(Time_min, Time_max, Time_res, packets, N);
SR = SurfaceRenderer(Time_min, Time_max, Time_res, packets, N);
% CR = CircleRenderer(Time_min, Time_max, Time_res, packets, N);
% CRT= CircleRenderer_timeRandomized(Time_min, Time_max, Time_res, packets, N);
CRR= CircleRenderer_radiusRandomized(Time_min, Time_max, Time_res, packets, N);

figure, plot(CR./SR), title('CRR/SR');
% figure, plot(CRT./SR), title('CRT/SR');
% figure, plot(CRR./SR), title('CRR/SR');
toc