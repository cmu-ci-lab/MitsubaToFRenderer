% Carl Rasmussen clearly wasn't thinking of functions on
% one-million-dimensional space when he wrote his checkgrad routine :)
% This is a modified version of his code that uses a single random
% direction to check the gradient vs. change in objective.  It only
% calls the gradient/objective function three times, so it finishes a
% bit more quickly than CR's code.  This code prints and returns the
% ratio of change in objective to (normalized) directional derivative.
% Correct objective/gradient code will yield a ratio very close to
% unity.
%
% NOTE: Not reliable if x is close to a local min/max!
%
% function [r] = checkgrad2(ogfun,x,ogparams,varargin)
% ogfun - point to gradient/objective function
% x - point at which to test f [d,1]
% e - epsilon; a small number (e.g. 1e-10)
% r - ratio of change in objective to (scaled) directional gradient
%
% Written by Jason Rennie, March 2005
% Last modified: Tue Aug  8 22:23:47 2006

function [r] = checkgrad2(ogfun,x,ogparams,varargin)
  % Parameters to be set via varargin
  e = 1e-6;
  % Process varargin
  paramgt;
  
  n = length(x);
  [obj,dx] = ogfun(x,ogparams{:});
  d = e.*sign(2.*rand(n,1)-1);
  [obj2,dx2] = ogfun(x+d,ogparams{:});
  [obj1,dx1] = ogfun(x-d,ogparams{:});
  r = (obj2 - obj1)/(2.*(d'*dx));
  % if ratio is zero, e is set too small
  fprintf(1,'e=%.0e ratio=%.10e (should be extremely close to unity)\n',e,r);

% ChangeLog
% 3/19/05 - Make all components of direction equal magnitude; otherwise,
% numerical issues may arise
% 3/19/05 - Too small of an 'e' value can be bad; 1e-6 works well for me
% 3/19/05 - "Modernize" parameter passing
% 3/17/05 - Choose a random direction rather than calculating
% change in each axis-parallel direction
