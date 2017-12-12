function [thetamin, thetamax, indices] = circlePolygonIntersectionAngles(points,r)


%% For all changes, test random, test one point on circle, two points on circle, three points on circle

norm_p = sqrt(sum(points.^2));

% -1 if inside circle, 0 if on the circle, +1 if outside circle
locations_p = nan(size(norm_p));

for i=1:length(points)
    checks = 0;
    if(almostEqual(norm_p, r))
        locations_p = -1;
        checks = checks + 1;
    end
    if(epsLess(norm_p,r))
        locations_p = -1;
        checks = checks + 1;
    end
    if(epsGreater(norm_p,r))
        locations_p = -1;
        checks = checks + 1;
    end
    if(isnan(locations_p))
        disp('Point location in circle-poly intersection is not determined');        
    end
    
    if(checks ~= 1)
        disp('Point location in circle-poly intersection is not clear');        
    end
end

% r  = .81;
% r = rand;
thetamin = zeros(4,1);
thetamax = zeros(4,1);
indices = 0;


theta  = [];
direction = []; % 1 is outside, -1 is inside

xIntersects = [];
yIntersects = [];


for i=1:length(points)
    j=i+1;
    if i==length(points)
        j=1;
    end
    
    if(locations_p(i) < r && norm_p(j) < r) %% both points inside, do nothing
        continue;
    end
    
    if(norm_p(i) < r && norm_p(j) > r) 
        direction(end+1) = 1;
        [theta(end+1), xIntersects(end+1), yIntersects(end+1)] = circleLineIntersection(points(:,i), points(:,j), r);
        continue;
    end
    if(norm_p(i) < r && norm_p(j) == r) 
        direction(end+1)    =  1;
        theta(end+1)        = atan2(points(2,j), points(1,j));
        xIntersects(end+1)  = points(1,j);
        yIntersects(end+1)  = points(2,j);
        continue;
    end
    
    if(norm_p(i) > r && norm_p(j) < r)
        direction(end+1) =-1;
        [theta(end+1), xIntersects(end+1), yIntersects(end+1)] = circleLineIntersection(points(:,i), points(:,j), r);
        continue;
    end
    if(norm_p(i) == r && norm_p(j) < r)
        direction(end+1)    = -1;
        theta(end+1)        = atan2(points(2,i), points(1,i));
        xIntersects(end+1)  = points(1,i);
        yIntersects(end+1)  = points(2,i);
        continue;
    end
    
    if(norm_p(i) == r && norm_p(j) == r)
        direction(end+1)    = -1;
        theta(end+1)        = atan2(points(2,i), points(1,i));
        xIntersects(end+1)  = points(1,i);
        yIntersects(end+1)  = points(2,i);
        direction(end+1)    =  1;
        theta(end+1)        = atan2(points(2,j), points(1,j));
        xIntersects(end+1)  = points(1,j);
        yIntersects(end+1)  = points(2,j);
        continue;
    end
    
    if(norm_p(i) == r && norm_p(j) > r)
        [temp, noOfSols] = SpecialcircleLineIntersection(points(:,i), points(:,j), r, 2);
%         [alpha1, alpha2, theta1, theta2, noOfSols] = circleLineIntersection2(points(:,i), points(:,j), r, 2);
%         if(alpha1 <0.95) %Ideally should be 1
%             disp('Error in circle-line intersection. Point is on the circle but the intersection returns that it is not');
%         end
        if noOfSols == 2
            direction(end+1)    = -1;
            theta(end+1)        = atan2(points(2,i), points(1,i));
            xIntersects(end+1)  = points(1,i);
            yIntersects(end+1)  = points(2,i);
            
            direction(end+1)    =  1;
            theta(end+1)        = temp;
            xIntersects(end+1)  = r*cos(temp);
            yIntersects(end+1)  = r*sin(temp);
        else
            direction(end+1)    =  1;
            theta(end+1)        = atan2(points(2,i), points(1,i));
            xIntersects(end+1)  = points(1,i);
            yIntersects(end+1)  = points(2,i); 
        end
%         if(numberOfCircleLineIntersections(points(:,i), points(:,j), r) == 2)
%             direction(end+1)    = -1;
%             theta(end+1)        = atan2(points(2,i), points(1,i));
%             xIntersects(end+1)  = points(1,i);
%             yIntersects(end+1)  = points(2,i);    
%             
%             direction(end+1)    =  1;
%             [theta(end+1), xIntersects(end+1), yIntersects(end+1)] = circleLineIntersection(points(:,i), points(:,j), r);            
%         else
%             %Going outside
%             direction(end+1)    = 1;
%             theta(end+1)        = atan2(points(2,i), points(1,i));
%             xIntersects(end+1)  = points(1,i);
%             yIntersects(end+1)  = points(2,i);            
%         end
        continue;
    end
    
    if(norm_p(i) > r && norm_p(j) == r)        
        
        [temp, noOfSols] = SpecialcircleLineIntersection(points(:,i), points(:,j), r, 1);
%         [alpha1, alpha2, theta1, theta2, noOfSols] = circleLineIntersection2(points(:,i), points(:,j), r, 1);
%         if(alpha2 >0.05) %Ideally should be 1
%             disp('Error in circle-line intersection. Point is on the circle but the intersection returns that it is not');
%         end
        if noOfSols == 2
            direction(end+1)    = -1;
            theta(end+1)        = temp;
            xIntersects(end+1)  = r*cos(temp);
            yIntersects(end+1)  = r*sin(temp);
            
            direction(end+1)    =  1;
            theta(end+1)        = atan2(points(2,j), points(1,j));
            xIntersects(end+1)  = points(1,j);
            yIntersects(end+1)  = points(2,j); 
        else
            %Going inside
            direction(end+1)    = -1;
            theta(end+1)        = atan2(points(2,j), points(1,j));
            xIntersects(end+1)  = points(1,j);
            yIntersects(end+1)  = points(2,j);     
        end
%         if(numberOfCircleLineIntersections(points(:,i), points(:,j), r) == 2)
%             direction(end+1)    = -1;
%             [theta(end+1), xIntersects(end+1), yIntersects(end+1)] = circleLineIntersection(points(:,i), points(:,j), r);            
% 
%             direction(end+1)    =  1;
%             theta(end+1)        = atan2(points(2,j), points(1,j));
%             xIntersects(end+1)  = points(1,j);
%             yIntersects(end+1)  = points(2,j);                
%         else
%             %Going outside
%             direction(end+1)    = -1;
%             theta(end+1)        = atan2(points(2,j), points(1,j));
%             xIntersects(end+1)  = points(1,j);
%             yIntersects(end+1)  = points(2,j);            
%         end
        continue;
    end
    
    if(norm_p(i) > r && norm_p(j) > r)
        %compute projection of origin on the line joining p[i] and p[j]
        p1 = points(:,i);
        p2 = points(:,j);
        
        n = [p2(2)-p1(2) p1(1)-p2(1)];
        n = n/norm(n);
        P_0 = dot(p1,n) * n;

        % If projection is not in the circle, the line-segment is completely outside circle
        if(norm(P_0) > r)
            continue;
        end
        % compute where the projection is, with respect to the line joining p[i] and p[j]
        if p1(1)~=p2(1)
            alpha = (P_0(1)-p2(1))/(p1(1)-p2(1));
        else
            alpha = (P_0(2)-p2(2))/(p1(2)-p2(2));
        end
        
        %if the projection is outside the line joining p[i] and p[j], then the the line segment is completely outside circle
        if(alpha < 0 || alpha > 1)
            continue;            
        end
        
        direction(end+1) = -1;
        [theta(end+1), xIntersects(end+1), yIntersects(end+1)] = circleLineIntersection(p1, P_0, r);
        
        direction(end+1) =  1;
        [theta(end+1), xIntersects(end+1), yIntersects(end+1)] = circleLineIntersection(P_0, p2, r);
     
        continue;
    end
    disp('Shouldn''t have reached here');
end


% Wrap thetas and sort them
theta = wrapTo2Pi(theta);

[theta, inds] = sort(theta);
direction       = direction(inds);


%Delete redundancies
deleteIndices = [];

t_size = length(theta);
if(t_size == 2)
    t_size = 1;
end

for i=1:t_size
    j=i+1;
    if(j > length(theta))
        j=1;
    end
    if(theta(i) == theta(j))
        if(direction(i) == direction(j))
            deleteIndices(end + 1) = i;
        else
            deleteIndices(end + 1) = i;
            if(j~=1)
                deleteIndices(end + 1) = j;
            else
                deleteIndices = [1 deleteIndices];
            end
        end
        i = i + 1;
    end
end


theta(deleteIndices) = [];
direction(deleteIndices) = [];
indices = length(theta);


if(isempty(direction))
    %% check that the corners of triangle are outside the circle
    if(norm_p(1) <= r || norm_p(2) <= r || norm_p(3) <= r)
        return;
    end
    
    %% Entire circle might be inside triangle. Check if the origin is inside triangle
    v0 = points(:,1);
    v1 = points(:,2)-points(:,1);
    v2 = points(:,3)-points(:,1);
    a = -det([v0 v2])/det([v1 v2]);
    b =  det([v0 v1])/det([v1 v2]);
    
    if( a>0 && b>0 && a+b <1)
        thetamin = 0;
        thetamax = 2*pi;
        indices = 1;
    end
    return;
end

index = 1;
if(isempty(direction))
    return;
end

%% Find the normal of the plane  
N = cross([points(:,2)-points(:,1);0], [points(:,3)-points(:,1);0]);
% N  = zeros(3,1);
% 
% for i=1:size(points,2)
%     j=i+1;
%     if(i==size(points,2))
%         j=1;
%     end
%     N = N + cross([points(:,i);0],[points(:,j);0]);
% end

%% Compute the angles
start = 1;
if (xor(direction(1)>0,N(3)>0))
    start = 2;
end

for i=start:2:length(theta) 
    thetamin(index) = theta(i);
    j = i+1;
    if(j>length(theta))
        j=1;
    end
    thetamax(index) = theta(j);
    if(thetamax(index) < thetamin(index))
        thetamax(index + 1) = thetamax(index);
        thetamax(index) = 2*pi;
        thetamin(index + 1) = 0;
        index = index+1;
    end
    index = index + 1;
end
indices = index-1;

function x = almostEqual(a, b)
    x = false;
    if( (a-b) < 1e-7 && (b-a) < 1e-7 )
        x = true;
    end
end

function x = epsGreater(a, b)
    x = false;
    if( a > (b + 1e-7) )
        x = true;
    end
end

function x = epsLess(a, b)
    x = false;
    if( a < (b - 1e-7) )
        x = true;
    end
end

