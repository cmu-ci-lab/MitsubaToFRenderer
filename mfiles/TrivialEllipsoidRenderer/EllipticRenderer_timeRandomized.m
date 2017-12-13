function Transient = EllipticRenderer_timeRandomized(Time_min, Time_max, Time_res, packets, N)


p1 = [1, 0, 0];
p2 = [0, 0, 0];



x_min_plane = -1;
x_max_plane =  1;
y_min_plane = -1;
y_max_plane =  1;
z_plane = 2;

v1 = [x_min_plane y_min_plane z_plane 1]';
v2 = [x_min_plane y_max_plane z_plane 1]';
v3 = [x_max_plane y_max_plane z_plane 1]';
v4 = [x_max_plane y_min_plane z_plane 1]';


Polygon1 = [v1 v2 v3];
Polygon2 = [v3 v4 v1];

Time = Time_min:Time_res:Time_max;

Transient = zeros(size(Time));

for i=1:packets
    Times = Time_min + (Time_max - Time_min)*rand(N,1);
    T = Times;    
    T(T < sqrt(17)) = [];
%     T(T > 12) = [];
    Intensities = zeros(length(T),1);
    
    for j=1:length(T)
        if(rand < 0.5)
            [P, measure, intersects] = EllipsoidTriangleIntersectionSamplingFunction(p1, p2, T(j), Polygon1);
        else
            [P, measure, intersects] = EllipsoidTriangleIntersectionSamplingFunction(p1, p2, T(j), Polygon2);
        end
        if(~intersects)
            continue;
        end
        x_plane = P(1);
        y_plane = P(2);
        z_plane = P(3);
        Intensities(j) = (cos(atan((sqrt((x_plane-1).^2+y_plane.^2))./z_plane))./((x_plane-1).^2+y_plane.^2+z_plane.^2)).* ...
              (cos(atan((sqrt(x_plane.^2+y_plane.^2))./z_plane))./(x_plane.^2+y_plane.^2+z_plane.^2)).*measure*(Time_max - Time_min)*2;
        %%verify
%         sqrt((x_plane-1).^2+y_plane.^2+z_plane.^2) + sqrt((x_plane).^2+y_plane.^2+z_plane.^2) - T(j)
    end
    Times_indices = ceil((T - Time_min)/Time_res);
    u = unique(Times_indices);
    for j = 1:length(u)
       Transient(u(j)) = Transient(u(j)) + sum(Intensities(Times_indices == u(j)));
    end
end

Transient = Transient/(N*packets);