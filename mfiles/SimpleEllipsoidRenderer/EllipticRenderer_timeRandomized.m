function Transient = EllipticRenderer_timeRandomized(x_min_plane, x_max_plane, y_min_plane, y_max_plane, z_plane, p1, p2, Time_min, Time_max, Time_res, packets, N)

v1 = [x_min_plane y_min_plane z_plane 1]';
v2 = [x_min_plane y_max_plane z_plane 1]';
v3 = [x_max_plane y_max_plane z_plane 1]';
v4 = [x_max_plane y_min_plane z_plane 1]';

Polygon1 = [v3 v1 v4];
Polygon2 = [v3 v2 v1];

Time = Time_min:Time_res:Time_max;

Transient = zeros(size(Time));

for i=1:packets
    Times = Time_min + (Time_max - Time_min)*rand(N,1);
    T = Times;    
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
        D_1 = p1' - P(1:3);
        D_2 = p2' - P(1:3);
        Intensities(j) = cos( atan(norm(D_1(1:2))/D_1(3)) )./(norm(D_1)^2).* ...
                         cos( atan(norm(D_2(1:2))/D_2(3)) )./(norm(D_2)^2).* ...
                            measure*(Time_max - Time_min)*2;
%         Intensities(j) = (cos(atan((sqrt((x_plane-p1(1)).^2+y_plane.^2))./z_plane))./((x_plane-1).^2+y_plane.^2+z_plane.^2)).* ...
%               (cos(atan((sqrt(x_plane.^2+y_plane.^2))./z_plane))./(x_plane.^2+y_plane.^2+z_plane.^2)).*measure*(Time_max - Time_min)*2;
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