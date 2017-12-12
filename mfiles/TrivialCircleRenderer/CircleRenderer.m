function Transient = CircleRenderer(Time_min, Time_max, Time_res, packets, N)

Time = Time_min:Time_res:Time_max;
Transient = gpuArray.zeros(size(Time));

x_min_plane = -1;
x_max_plane =  1;
y_min_plane = -1;
y_max_plane =  1;
z_plane = 2;

for j=1:packets
    for i=1:length(Time)
        T = Time(i)/2;
        if(T < z_plane)
            continue;
        end
        r = sqrt(T^2-z_plane^2);
        theta = 2*pi*rand(N,1);

        x_plane = r*cos(theta);
        y_plane = r*sin(theta);

        indices = (x_plane < x_min_plane) | (x_plane > x_max_plane) | (y_plane < y_min_plane) | (y_plane > y_max_plane);

        x_plane(indices) = [];
        y_plane(indices) = [];

    %     lengths = 2*pi*r.*(r<1) + 4*r*(pi/2-2*acos(1/(r+eps))).*(r>=1);
    %     Areas = 2*pi*T*T*sin(atan((sqrt(x_plane.^2+y_plane.^2))./z_plane)).*(r<1) + 4*T*T*(pi/2-2*acos(1/(r+eps)))*sin(atan((sqrt(x_plane.^2+y_plane.^2))./z_plane)).*(r>=1) ;
        Intensities = (cos(atan((sqrt(x_plane.^2+y_plane.^2))./z_plane))./(x_plane.^2+y_plane.^2+z_plane.^2)).^2*2*pi*T*Time_res/2;


        Transient(i) = sum(Intensities);
    end
end
Transient = Transient/(N*packets);
