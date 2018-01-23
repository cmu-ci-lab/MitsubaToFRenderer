function Transient = PhotonEllipticRenderer(x_min_plane, x_max_plane, y_min_plane, y_max_plane, z_plane, p1, p2, Time_min, Time_max, Time_res, packets, N)

Time = Time_min:Time_res:Time_max;
Transient = gpuArray.zeros(size(Time));

for i=1:packets
    phi = 2*pi*gpuArray.rand(N,1);
    theta = acos(2*gpuArray.rand(N,1)-1);

    direction = [sin(theta).*cos(phi) sin(theta).*sin(phi) cos(theta)];

%     direction(direction(:,3)<=0,:) = [];

    % Find intersection of the ray

    ray_length = (z_plane-p2(3))./direction(:,3);
    direction(ray_length <= 0,:) = [];
    ray_length(ray_length <= 0) = [];

    x_plane = ray_length.*direction(:,1) + p2(1);
    y_plane = ray_length.*direction(:,2) + p2(2);

    indices = (x_plane < x_min_plane) | (x_plane > x_max_plane) | (y_plane < y_min_plane) | (y_plane > y_max_plane);

    x_plane(indices) = [];
    y_plane(indices) = [];

    Intensities =4*pi*cos(atan(sqrt((x_plane-p1(1)).^2+(y_plane-p1(2)).^2)./(z_plane-p1(3))))./((x_plane-p1(1)).^2+(y_plane-p1(2)).^2+(z_plane-p1(3)).^2);
    Times = sqrt( (x_plane-p1(1)).^2+(y_plane-p1(2)).^2+(z_plane-p1(3)).^2 )+sqrt( (x_plane-p2(1)).^2+(y_plane-p2(2)).^2+(z_plane-p2(3)).^2 );
    
    Intensities(Times<=Time_min | Times>=Time_max) = [];
    Times(Times<=Time_min | Times>=Time_max) = [];
    
    Times_indices = ceil((Times - Time_min)/Time_res);
    u = unique(Times_indices);
    for j = 1:length(u)
       Transient(u(j)) = Transient(u(j)) + sum(Intensities(Times_indices == u(j)));
    end
end

Transient = Transient/(N*packets);
