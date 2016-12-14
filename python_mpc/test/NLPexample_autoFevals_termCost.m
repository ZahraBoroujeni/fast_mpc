y% Example script for getting started with FORCES NLP solver.
%
%--------------------------------------------------------------------------
% NOTE: This example shows how to define a nonlinear programming problem,
% where all derivative information is automatically generated by
% using the AD tool CasADi.
% 
% You may want to directly pass C functions to FORCES, for example when you 
% are using other AD tools, or have custom C functions for derivate 
% evaluation etc. See the example file "NLPexample_ownFevals.m" for how to
% pass custom C functions to FORCES.
%--------------------------------------------------------------------------
%
% This example solves an optimization problem for a car with the simple
% continuous-time, nonlinear dynamics
%
%    dx/dt = v*cos(theta)
%    dy/dt = v*sin(theta)
%    dv/dt = F/m
%    dtheta/dt = s/I
%
% where x,y are the position, v the velocity in heading angle theta of the
% car. The inputs are F (accelerating force) and steering s.
%
% The car starts from standstill with a certain heading angle, and the
% optimization problem is to maximize progress in y direction at the end
% of the prediction horizon while staying inside a non-convex feasible region.
%
% Quadratic costs for the acceleration force and steering are added to the
% objective to avoid excessive maneouvers.
%
% There are bounds on all variables.
%
% Variables are collected stage-wise into z = [F s x y v theta].
%
% See also FORCES_NLP
%
% (c) embotech GmbH, Zurich, Switzerland, 2013-16.

clear; clc; close all;
deg2rad = @(deg) deg/180*pi; % convert degrees into radians
rad2deg = @(rad) rad/pi*180; % convert radians into degrees

%% Problem dimensions
model.N = 50;            % horizon length
model.nvar = 6;          % number of variables
model.neq  = 4;          % number of equality constraints
model.nh = 2;            % number of inequality constraint functions

%% Objective function 
% In this example, we want to penalize the inputs F and s:
model.objective = @(z) 0.1*z(1)^2 + 0.01*z(2)^2;

% and maximize the progress on the y direction, while ensuring a small
% velocity and heading angle at the end of the horizon.
% Terminal cost: -100*y 100*v^2 + 100*theta^2 to aim for max y, v=0 and theta=0
model.objectiveN = @(z) -100*z(4) + 10*(z(5)-0)^2 + 10*(z(6)-0)^2;

% You can use standard Matlab handles to define these functions, i.e. you
% could also have this in a separate file. We use anonymous handles here
% only for convenience.

%% Dynamics, i.e. equality constraints 
% We use an explicit RK4 integrator here to discretize continuous dynamics:
m=1; I=1; % physical constants of the model
integrator_stepsize = 0.1;
continuous_dynamics = @(x,u) [x(3)*cos(x(4));  % v*cos(theta)
                              x(3)*sin(x(4));  % v*sin(theta)
                              u(1)/m;          % F/m
                              u(2)/I];         % s/I
model.eq = @(z) RK4( z(3:6), z(1:2), continuous_dynamics, integrator_stepsize);

% Indices on LHS of dynamical constraint - for efficiency reasons, make
% sure the matrix E has structure [0 I] where I is the identity matrix.
model.E = [zeros(4,2), eye(4)];

%% Inequality constraints
% upper/lower variable bounds lb <= x <= ub
%            inputs    |        states
%             F    s       x     y     v    theta
model.lb = [ -5,  -1,      -3,    0    0     0  ];
model.ub = [ +5,  +1,       0,    3    2    +pi ];

% General (differentiable) nonlinear inequalities hl <= h(x) <= hu
model.ineq = @(z)  [   z(3)^2 + z(4)^2; 
                    (z(3)+2)^2 + (z(4)-2.5)^2 ];
                
% Upper/lower bounds for inequalities
model.hu = [9,  +inf]';
model.hl = [1,  0.95^2]';

%% Initial and final conditions

% Initial condition on vehicle states
model.xinit = [-2, 0, 0, deg2rad(120)]'; % x=-2, y=0, v=0 (standstill), heading angle=120?
model.xinitidx = 3:6; % use this to specify on which variables initial conditions are imposed


%% Define solver options
codeoptions = getOptions('FORCESNLPsolver');
codeoptions.maxit = 500;    % Maximum number of iterations
codeoptions.printlevel = 0; % Use printlevel = 2 to print progress (but not for timings)
codeoptions.optlevel = 0;   % 0: no optimization, 1: optimize for size, 2: optimize for speed, 3: optimize for size & speed

% change this to your server or leave uncommented for using the standard
% embotech server at https://www.embotech.com/codegen
% codeoptions.server = 'http://embotech-server2.ee.ethz.ch:8114/v1.5.beta'; 

%% Generate forces solver
FORCES_NLP(model, codeoptions);

%% Call solver
% Set initial guess to start solver from:
x0i=model.lb+(model.ub-model.lb)/2;
x0=repmat(x0i',model.N,1);
problem.x0=x0; 

% Set initial condition. This is usually changing from problem
% instance to problem instance:
problem.xinit = model.xinit;

% Time to solve the NLP!
[output,exitflag,info] = FORCESNLPsolver(problem);

% Make sure the solver has exited properly. 
assert(exitflag == 1,'Some problem in FORCES solver');
fprintf('\nFORCES took %d iterations and %f seconds to solve the problem.\n',info.it,info.solvetime);

%% Plot results
TEMP = zeros(model.nvar,model.N);
for i=1:model.N
    TEMP(:,i) = output.(['x',sprintf('%02d',i)]);
end
U = TEMP(1:2,:);
X = TEMP(3:6,:);

% plot trajectory
figure(1); clf;
plot(X(1,:),X(2,:),'b-'); hold on; 
rectangle('Position',[-sqrt(model.hl(1)) -sqrt(model.hl(1)) 2*sqrt(model.hl(1)) 2*sqrt(model.hl(1))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
rectangle('Position',[-sqrt(model.hu(1)) -sqrt(model.hu(1)) 2*sqrt(model.hu(1)) 2*sqrt(model.hu(1))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
rectangle('Position',[-2-sqrt(model.hl(2)) 2.5-sqrt(model.hl(2)) 2*sqrt(model.hl(2)) 2*sqrt(model.hl(2))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
plot(model.xinit(1),model.xinit(2),'bx','LineWidth',3); 
title('position'); xlim([-2.5 0]); ylim([0 3]); xlabel('x position'); ylabel('y position');

% plot heading angle and velocity variables
figure(2); clf;
subplot(2,1,1); plot(X(3,:)); grid on; title('velocity'); hold on; 
plot([1 model.N], [model.ub(5) model.ub(5)]', 'r:');
plot([1 model.N], [model.lb(5) model.lb(5)]', 'r:');
subplot(2,1,2); plot(rad2deg(X(4,:))); grid on; title('heading angle'); ylim([0, 180]); hold on; 
plot([1 model.N], rad2deg([model.ub(6) model.ub(6)])', 'r:');
plot([1 model.N], rad2deg([model.lb(6) model.lb(6)])', 'r:');

% plot inputs
figure(3); clf;
subplot(2,1,1); stairs(U(1,:)); grid on; title('acceleration force'); hold on; 
plot([1 model.N], [model.ub(1) model.ub(1)]', 'r:');
plot([1 model.N], [model.lb(1) model.lb(1)]', 'r:');
subplot(2,1,2); stairs(U(2,:)); grid on; title('delta steering'); hold on; 
plot([1 model.N], [model.ub(2) model.ub(2)]', 'r:');
plot([1 model.N], [model.lb(2) model.lb(2)]', 'r:');