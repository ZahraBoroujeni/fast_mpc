% Example script for getting started with FORCES NLP solver.
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
% optimization problem is to maximize progress in y direction while staying
% inside a non-convex feasible region.
%
% Quadratic costs for the acceleration force and steering are added to the
% objective to avoid excessive maneouvers.
%
% There are bounds on all variables.
%
% Variables are collected stage-wise into z = [F s x y v theta].
% Parameters are collected into p = [m I]
%
% See also FORCES_NLP
%
% (c) embotech GmbH, Zurich, Switzerland, 2013-16.

clear; clc; close all;
deg2rad = @(deg) deg/180*pi; % convert degrees into radians
rad2deg = @(rad) rad/pi*180; % convert radians into degrees

%% Problem dimensions
model.N = 85;           % horizon length
model.nvar = 12;         % number of variables
model.neq  = 8;         % number of equality constraints
model.nh = 3;           % number of inequality constraint functions
model.npar = 2;         % number of parameters

%% Objective function 
% In this example, we want to maximize position in y direction,
% with some penalties on the inputs F and s:
model.objective = @(z) 0.1*z(1)^2 + 0.01*z(2)^2 + 0.1*z(3)^2 + 0.01*z(4)^2  + 0.01*(z(5)^2+z(6)^2-2.25)^2;
model.objectiveN = @(z) 100*(z(5)-1.5)^2 + 100*(z(6)-0)^2;

% You can use standard Matlab handles to define these functions, i.e. you
% could also have this in a separate file. We use anonymous handles here
% only for convenience.

%% Dynamics, i.e. equality constraints 
% We use an explicit RK4 integrator here to discretize continuous dynamics:
m=1; I=1; % physical constants of the model
integrator_stepsize = 0.1;
continuous_dynamics = @(x,u,p) [x(3)*cos(x(4));  % v*cos(theta)
                                x(3)*sin(x(4));  % v*sin(theta)
                                u(1)/p(1);       % F/m
                                u(2)/p(2);       % s/I
                                x(7)*cos(x(8));  % x_obst=v*cos(theta)
                                x(7)*sin(x(8));  % y_obst=v*sin(theta)
                                u(3);            % F_obst
                                u(4)];           % s_obst
                                
                                
model.eq = @(z,p) RK4( z(5:12), z(1:4), continuous_dynamics, integrator_stepsize, p);

% Indices on LHS of dynamical constraint - for efficiency reasons, make
% sure the matrix E has structure [0 I] where I is the identity matrix.
model.E = [zeros(8,4), eye(8)];

%% Inequality constraints
% upper/lower variable bounds lb <= x <= ub
%            inputs |inputs obs|     states  |  state obstacles
%             F   s | F   s | x  y  v theta| x y v theta ,
model.lb = [ -5, -1, -0.01,-1,-3, -1, 0, -pi,-3 0 0 -pi ];
model.ub = [ +5, +1, +0.01,+1, 3, 3, 2, +pi , 3 3 1 +pi];

% General (differentiable) nonlinear inequalities hl <= h(x) <= hu
% tanalpha=-2*z(9)/(2*(2.25-z(9)^2));
% alpha= atan2(-2*z(9)/(2*(2.25-z(9)^2)),1);
% (cos(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))^2+sin(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))^2/3)*(z(5)-z(9))^2-2*cos(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))*sin(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))*(1/(a^2)-1/(b^2))+(sin(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))^2+cos(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))^2/3)*(z(6)-z(10))^2
model.ineq = @(z)  [z(5)^2 + z(6)^2;
     (z(9)^2+z(10)^2-2.25);
    ((cos(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))*(z(5)-z(9))+sin(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))*(z(6)-z(10)))^2)/((0.1+z(9))^2)+((sin(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))*(z(5)-z(9))-cos(atan2(-2*z(9)/(2*(2.25-z(9)^2)),1))*(z(6)-z(10)))^2)/(0.25)];
    % (z(5)-z(9))^2+(z(6)-z(10))^2    
    
% Upper/lower bounds for inequalities
model.hu = [9,0.1,inf];
model.hl = [2,-0.1,1];

%% Initial and final conditions

% Initial condition on vehicle states
model.xinit = [-1.5, 0, 0.4, deg2rad(90),-1, 1.11, 0.1, deg2rad(45)]'; % x=-2, y=0, v=0 (standstill), heading angle=120?
model.xinitidx = 5:12; % use this to specify on which variables initial conditions are imposed

% Final condition on vehicle velocity and heading angle
% model.xfinal = deg2rad(0); % heading angle final=0?
% model.xfinalidx = 8; % use this to specify on which variables final conditions are imposed
% model.xfinal = 0; % x final=0
% model.xfinalidx = 5; 
% model.xfinal = [1.5 0]'; % x final=0 %1.4 0.53
%  model.xfinalidx = 5:6; 
%% Define solver options
codeoptions = getOptions('FORCESNLPsolver');
codeoptions.maxit = 2000;    % Maximum number of iterations
codeoptions.printlevel = 2; % Use printlevel = 2 to print progress (but not for timings)
codeoptions.optlevel =0;   % 0: no optimization, 1: optimize for size, 2: optimize for speed, 3: optimize for size & speed
codeoptions.noVariableElimination = 1;
codeoptions.nlp.lightCasadi = 1;
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

% Set initial and final conditions. This is usually changing from problem
% instance to problem instance:
problem.xinit = model.xinit;
% problem.xfinal = model.xfinal;

% Set parameters
problem.all_parameters = repmat([m I]',model.N,1);

% Time to solve the NLP!
[output,exitflag,info] = FORCESNLPsolver(problem);

% Make sure the solver has exited properly. 
fprintf('\nFORCES exitflag %d.\n',exitflag);
% if (exitflag~=1)
%   model.objective = @(z) 0.01*z(1)^2 + 0.1*z(2)^2 + (z(5)^2+z(6)^2-4)^2;
%   FORCES_NLP(model, codeoptions);
%   [output,exitflag,info] = FORCESNLPsolver(problem);
% end;
 assert(exitflag == 1,'Some problem in FORCES solver');
fprintf('\nFORCES took %d iterations and %f seconds to solve the problem.\n',info.it,info.solvetime);

%% Plot results
TEMP = zeros(model.nvar,model.N);
for i=1:model.N
    TEMP(:,i) = output.(['x',sprintf('%02d',i)]);
end
U = TEMP(1:4,:);
X = TEMP(5:12,:);

% plot trajectory
% figure(1); clf;
% plot(X(1,:),X(2,:),'b-'); hold on; 
% plot(X(5,:),X(6,:),'g-'); hold on; 
% rectangle('Position',[-sqrt(1) -sqrt(1) 2*sqrt(1) 2*sqrt(1)],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
% rectangle('Position',[-sqrt(model.hl(1)) -sqrt(model.hl(1)) 2*sqrt(model.hl(1)) 2*sqrt(model.hl(1))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
% rectangle('Position',[-sqrt(model.hu(1)) -sqrt(model.hu(1)) 2*sqrt(model.hu(1)) 2*sqrt(model.hu(1))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
% rectangle('Position',[-sqrt(2.25) -sqrt(2.25) 2*sqrt(2.25) 2*sqrt(2.25)],'Curvature',[1 1],'EdgeColor','b','LineStyle',':');
% rectangle('Position',[-sqrt(4) -sqrt(4) 2*sqrt(4) 2*sqrt(4)],'Curvature',[1 1],'EdgeColor','b','LineStyle',':');
% rectangle('Position',[-sqrt(6.25) -sqrt(6.25) 2*sqrt(6.25) 2*sqrt(6.25)],'Curvature',[1 1],'EdgeColor','b','LineStyle',':');
% % for i=1:model.N
% % rotationAngle=atan2(-2*X(5,i)/(2*(2.25-X(5,i)^2)),1);
% % rotationArray = [cos(rotationAngle), sin(rotationAngle); -sin(rotationAngle), cos(rotationAngle)];
% % a=1;
% % b=0.5;
% % x=[-a,a];
% % y=[0,0];
% % 
% % vertix=[x',y']*rotationArray+[X(5,i),X(6,i);X(5,i),X(6,i)];
% %  x1=vertix(1,1);
% %  x2=vertix(2,1);
% %  y1=vertix(1,2);
% %  y2=vertix(2,2);
% %  a = 1/2*sqrt((x2-x1)^2+(y2-y1)^2);
% %  t = 0 : 0.01 : 2*pi;
% %  Xe = a*cos(t);
% %  Ye = b*sin(t);
% %  w = atan2(y2-y1,x2-x1);
% %  x = (x1+x2)/2 + Xe*cos(w) - Ye*sin(w);
% %  y = (y1+y2)/2 + Xe*sin(w) + Ye*cos(w);
% %  plot(x,y,'c')
% % %plot(vertix(:,1)+X(5,i),vertix(:,2)+X(6,i),'c')
% % end
% % plot(x,y,'r')
%  
% %rectangle('Position',[-2-sqrt(model.hl(2)) 2.5-sqrt(model.hl(2)) 2*sqrt(model.hl(2)) 2*sqrt(model.hl(2))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
% plot(model.xinit(1),model.xinit(2),'bx','LineWidth',3); 
% title('position'); %xlim([-2.5 3]); ylim([0 3]); 
% xlabel('x position'); ylabel('y position');
% 
% 
% % plot heading angle and velocity variables
% figure(2); clf;
% subplot(2,1,1); plot(X(3,:));  hold on;  plot(X(7,:)); grid on; title('velocity'); hold on; 
% plot([1 model.N], [model.ub(7) model.ub(7)]', 'r:');
% plot([1 model.N], [model.lb(7) model.lb(7)]', 'r:');
% subplot(2,1,2); plot(rad2deg(X(4,:))); hold on;plot(rad2deg(X(8,:)),'g-'); grid on; title('heading angle');  hold on; %ylim([0, 180]);
% plot([1 model.N], rad2deg([model.ub(8) model.ub(8)])', 'r:');
% plot([1 model.N], rad2deg([model.lb(8) model.lb(8)])', 'r:');
% 
% % plot inputs
% figure(3); clf;
% subplot(2,1,1); stairs(U(1,:)); grid on; title('acceleration force'); hold on; 
% plot([1 model.N], [model.ub(1) model.ub(1)]', 'r:');
% plot([1 model.N], [model.lb(1) model.lb(1)]', 'r:');
% subplot(2,1,2); stairs(U(2,:)); grid on; title('delta steering'); hold on; 
% plot([1 model.N], [model.ub(2) model.ub(2)]', 'r:');
% plot([1 model.N], [model.lb(2) model.lb(2)]', 'r:');

%%
% plot trajectory
figure(1); clf;
c = linspace(0.5,1,model.N);
r = linspace(0,0,model.N);
g = linspace(0,0,model.N);
scatter(X(1,:),X(2,:),14,[r',g',c'],'filled');hold on;

% map=[0,0,1]
% colormap(map)
c = linspace(0,0,model.N);
r = linspace(0,0,model.N);
g = linspace(0.5,1,model.N);
scatter(X(5,:),X(6,:),4,[r',g',c'],'filled');

scatter(X(5,33),X(6,33),100,2,'*')
scatter(X(1,33),X(2,33),100,2,'*')

rectangle('Position',[-sqrt(1) -sqrt(1) 2*sqrt(1) 2*sqrt(1)],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
rectangle('Position',[-sqrt(model.hl(1)) -sqrt(model.hl(1)) 2*sqrt(model.hl(1)) 2*sqrt(model.hl(1))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
rectangle('Position',[-sqrt(model.hu(1)) -sqrt(model.hu(1)) 2*sqrt(model.hu(1)) 2*sqrt(model.hu(1))],'Curvature',[1 1],'EdgeColor','r','LineStyle',':');
rectangle('Position',[-sqrt(2.25) -sqrt(2.25) 2*sqrt(2.25) 2*sqrt(2.25)],'Curvature',[1 1],'EdgeColor','b','LineStyle',':');
rectangle('Position',[-sqrt(4) -sqrt(4) 2*sqrt(4) 2*sqrt(4)],'Curvature',[1 1],'EdgeColor','b','LineStyle',':');
rectangle('Position',[-sqrt(6.25) -sqrt(6.25) 2*sqrt(6.25) 2*sqrt(6.25)],'Curvature',[1 1],'EdgeColor','b','LineStyle',':');
box on
legend({'autonomous car','obstacle1','iteration number 33'},'FontSize',8,'FontWeight','bold','Location','best')
 for i=1:model.N
rotationAngle=atan2(-2*X(5,i)/(2*(2.25-X(5,i)^2)),1);
rotationArray = [cos(rotationAngle), sin(rotationAngle); -sin(rotationAngle), cos(rotationAngle)];
a=0.1+z(9);
b=0.5;
x=[-a,a];
y=[0,0];
vertix=[x',y']*rotationArray+[X(5,i),X(6,i);X(5,i),X(6,i)];
 x1=vertix(1,1);
 x2=vertix(2,1);
 y1=vertix(1,2);
 y2=vertix(2,2);
 a = 1/2*sqrt((x2-x1)^2+(y2-y1)^2);
 t = 0 : 0.01 : 2*pi;
 Xe = a*cos(t);
 Ye = b*sin(t);
 w = atan2(y2-y1,x2-x1);
 x = (x1+x2)/2 + Xe*cos(w) - Ye*sin(w);
 y = (y1+y2)/2 + Xe*sin(w) + Ye*cos(w);
 if (i==33)
    plot(x,y,'c')
 end
%plot(vertix(:,1)+X(5,i),vertix(:,2)+X(6,i),'c')
end
%plot(x,y,'r')
plot(model.xinit(1),model.xinit(2),'bx','LineWidth',3); 
title('position'); xlim([-3 3]); ylim([0 3]); xlabel('x position'); ylabel('y position');
set(gca,'fontsize',10)

% plot heading angle and velocity variables
figure(2); clf;
subplot(2,1,1); plot(X(3,:));  hold on;  plot(X(7,:),'g-');  hold on; grid on; title('Normalized velocity'); hold on; 
plot([1 model.N], [model.ub(9) model.ub(9)]', 'r:');
plot([1 model.N], [model.lb(9) model.lb(9)]', 'r:');
ylabel('Normalized velocity ');
ylim([0 1]);
set(gca,'fontsize',10)
legend({'V\_{autonomous car}','V\_{obstacle1}'},'FontSize',8,'FontWeight','bold','Location','best')
subplot(2,1,2); plot(rad2deg(X(4,:))); hold on;plot(rad2deg(X(8,:)),'g-'); grid on; title('heading angle'); ylim([0, 180]); hold on; 
plot([1 model.N], rad2deg([model.ub(10) model.ub(10)])', 'r:');
plot([1 model.N], rad2deg([model.lb(10) model.lb(10)])', 'r:');
 xlabel('iteration (10ms)'); ylabel('\theta (degree)');
 set(gca,'fontsize',10)
 legend({'\theta\_{autonomous car}','\theta\_{obstacle1}'},'FontSize',8,'FontWeight','bold','Location','best')
 ylim([-90 90]);
% plot inputs
figure(3); clf;
subplot(2,1,1); plot(U(1,:));hold on;  plot(U(3,:),'g-'); grid on; title('acceleration'); hold on; 
% plot([1 model.N], [model.ub(1) model.ub(1)]', 'r:');
% plot([1 model.N], [model.lb(1) model.lb(1)]', 'r:');
ylabel('acceleration (m/s^2)');
  set(gca,'fontsize',10)
 legend({'a\_{autonomous car}','a\_{obstacle1}'},'FontSize',8,'FontWeight','bold','Location','northeast');
  ylim([-0.5 0.5]);
subplot(2,1,2); plot(U(2,:));grid on; title('steering'); hold on; plot(U(4,:),'g-');
plot([1 model.N], [model.ub(2) model.ub(2)]', 'r:');
plot([1 model.N], [model.lb(2) model.lb(2)]', 'r:');
 xlabel('iteration (10ms)'); ylabel('steering (degree)');
  set(gca,'fontsize',10)
 legend({'s\_{autonomous car}','s\_{obstacle1}'},'FontSize',8,'FontWeight','bold','Location','northeast');

