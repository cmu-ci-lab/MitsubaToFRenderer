clc,clear, close all

points = [100 -100 -340.799988;
         -100  100 -340.799988;
          100  100 -340.799988;];
T = [1 2 3];

TR = triangulation(T, points);

Original = [100.0000008 29.7755661 -340.799988];

cartesianToBarycentric(TR,1,Original)