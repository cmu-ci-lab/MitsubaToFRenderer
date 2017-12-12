function theta = ellipticSample(e, theta_min, theta_max, indices)

cumsum = length(theta_min);
cumsum(1) = theta_max(1) - theta_min(1);
for i=2:indices
    cumsum(i) = cumsum(i-1) + theta_max(i) - theta_min(i);
end

while(1)
    theta_s = rand*cumsum(indices);
    if(theta_s < cumsum(1))
        theta = theta_s + theta_min(1);
    else
        for i=2:indices
            if(theta_s < cumsum(i))
                theta = theta_s - cumsum(i-1) + theta_min(i);
                break;
            end
        end
    end
    r = rand;
    if(r < sqrt(1-(e*cos(theta)^2)))
        return;
    end
end

end

