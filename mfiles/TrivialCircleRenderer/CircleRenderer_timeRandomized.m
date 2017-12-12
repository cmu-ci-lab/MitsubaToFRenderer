function Transient = CircleRenderer_timeRandomized(Time_min, Time_max, Time_res, packets, N)

Time = Time_min:Time_res:Time_max;

Transient = gpuArray.zeros(size(Time));

x_min_plane = -1;
x_max_plane =  1;
y_min_plane = -1;
y_max_plane =  1;
z_plane = 2;

for i=1:packets
    Times = Time_min + (Time_max - Time_min)*gpuArray.rand(N,1);
    T = Times/2;
    
    T(T < z_plane) = [];

    r = sqrt(T.^2-z_plane^2);
    theta = 2*pi*rand(size(r));
    
    x_plane = r.*cos(theta);
    y_plane = r.*sin(theta);
    
    indices = (x_plane < x_min_plane) | (x_plane > x_max_plane) | (y_plane < y_min_plane) | (y_plane > y_max_plane);

    x_plane(indices) = [];
    y_plane(indices) = [];
    T(indices) = [];
    r(indices) = [];
   
    Intensities = (cos(atan((sqrt(x_plane.^2+y_plane.^2))./z_plane))./(x_plane.^2+y_plane.^2+z_plane.^2)).^2 * 2*pi*(Time_max - Time_min).*T/2;
    
    Times_indices = ceil((2*T - Time_min)/Time_res);
    u = unique(Times_indices);
    for j = 1:length(u)
       Transient(u(j)) = Transient(u(j)) + sum(Intensities(Times_indices == u(j)));
    end
end

Transient = Transient/(N*packets);