function Transient = PhotonEllipticRenderer(Time_min, Time_max, Time_res, packets, N)
% Time_min = 3;
% Time_max = 6;
% Time_res = 0.01;

Time = Time_min:Time_res:Time_max;
Transient = gpuArray.zeros(size(Time));

x_min_plane = -1;
x_max_plane =  1;
y_min_plane = -1;
y_max_plane =  1;
z_plane = 2;

% packets = 1;
% N = 1e7; % Total rays = packets*N;

for i=1:packets
    phi = 2*pi*gpuArray.rand(N,1);
    theta = acos(gpuArray.rand(N,1));

    direction = [sin(theta).*cos(phi) sin(theta).*sin(phi) cos(theta)];

    direction(direction(:,3)<=0,:) = [];

    % Find intersection of the ray

    ray_length = z_plane./direction(:,3);

    x_plane = ray_length.*direction(:,1) + 1;
    y_plane = ray_length.*direction(:,2);

    indices = (x_plane < x_min_plane) | (x_plane > x_max_plane) | (y_plane < y_min_plane) | (y_plane > y_max_plane);

    x_plane(indices) = [];
    y_plane(indices) = [];

    Intensities =2*pi*cos(atan(sqrt(x_plane.^2+y_plane.^2)./z_plane))./(x_plane.^2+y_plane.^2+z_plane.^2);
    Times = sqrt(x_plane.^2+y_plane.^2+z_plane.^2) + sqrt((x_plane-1).^2+y_plane.^2+z_plane.^2);
    Times_indices = ceil((Times - Time_min)/Time_res);
    u = unique(Times_indices);
    for j = 1:length(u)
       Transient(u(j)) = Transient(u(j)) + sum(Intensities(Times_indices == u(j)));
    end
end

Transient = Transient/(N*packets);
