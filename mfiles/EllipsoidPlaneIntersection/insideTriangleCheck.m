clc,clear,close all


for i=1:100
    rng(i);
    points = 2*rand(2,3)-1;
    v0 = points(:,1);
    v1 = points(:,2)-points(:,1);
    v2 = points(:,3)-points(:,1);

    a = -det([v0 v2])/det([v1 v2]);
    b =  det([v0 v1])/det([v1 v2]);
    
    figure(1)
    points(:,end+1) = points(:, 1); 
    plot(points(1, :), points(2, :), 'k'); hold on

    if( a>0 && b>0 && a+b <1)
        plot(0,0,'g+');hold on;
    else
        plot(0,0,'r+');hold on;
    end
    xlim([-1 1]);
    ylim([-1 1]);
    pause(0.5);
    hold off
end


% rng(67);
% points = 2*rand(2,3)-1;
% 
% 
% 
% points(:,end+1) = points(:, 1); 
% plot(points(1, :), points(2, :), 'k'); hold on
% 
% for i=-3:.01:3
%     for j=-3:.01:3
%         r = rand;
% 
%         v0 = points(:,1);
%         v1 = points(:,2)-points(:,1);
%         v2 = points(:,3)-points(:,1);
% 
%         v = [i; j];
% 
%         a = (det([v v2]) - det([v0 v2]))/det([v1 v2]);
%         b = -(det([v v1]) - det([v0 v1]))/det([v1 v2]);
%         
%         if( a>0 && b>0 && a+b <1)
%             plot(i,j,'g.');hold on
%         else
%             plot(i,j,'b.');hold on;
%         end
%     end
% end