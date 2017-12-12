clear,
e = 1;
theta_min = 0;
theta_max = 2*pi;
indices   = 1;

for i=1:1000
    theta(i) = ellipticSample(e, theta_min, theta_max, indices);
end
figure(1)
% plot(cos(theta),0,'r.');hold off;

hist(cos(theta),10)