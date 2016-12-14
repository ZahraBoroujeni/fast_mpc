%% Explicit MPC Control of an Aircraft with Unstable Poles
% This example shows how to control an unstable aircraft with saturating
% actuators using Explicit MPC.
%
% Reference:
%
% [1] P. Kapasouris, M. Athans and G. Stein, "Design of feedback control 
% systems for unstable plants with saturating actuators", Proc. IFAC Symp. 
% on Nonlinear Control System Design, Pergamon Press, pp.302--307, 1990
%
% [2] A. Bemporad, A. Casavola, and E. Mosca, "Nonlinear control of
% constrained linear systems via predictive reference management", IEEE(R)
% Trans. Automatic Control, vol. AC-42, no. 3, pp. 340-349, 1997.
%
% See also MPCAIRCRAFT.

% Copyright 1990-2014 The MathWorks, Inc.  

%% Define Aircraft Model
% The linear open-loop dynamic model is as follows:
A = [1 0 0 0 0 0 0; 0 1 0 0 0 0 0; 0 0.1 1 0 0 0 0; 0 0 0 1 0 0 0; 0 0 0 0.1 1 0 0; 0 0 0 0 0 1 0; 0 0 0 0 0 0.1 1];
B=[1 0; 0 1; 0 0;1 0; 0 0; 0 1;0 0];
C = [1 0 0 0 0 0 0; 0 1 0 0 0 0 0; 0 0 1 0 0 0 0];
D = zeros(3,2);
plant = ss(A,B,C,D)
x0 = [20;0;0;10;-100;0;0]
%%
% The manipulated variables are the elevator and flaperon angles, the
% attack and pitch angles are measured outputs to be regulated. 
%
% The open-loop response of the system is unstable.
pole(plant)

%% Design MPC Controller
% To obtain an Explicit MPC controller, you must first design a traditional
% MPC (also referred as Implicit MPC) that is able to achieves your control
% objectives.

% *MV Constraints* 
%%
% All manipulated variables are constrained between ...  umin, umax
MV = struct('Min',{-4,-2},'Max',{1,2},'ScaleFactor',{50,50});

%%
% *OV Constraints* 
%%
% Both plant outputs have constraints to limit undershoots at the first
% prediction horizon.  You also specify scale factors for outputs.
OV = struct('Min',{0,-5,0},'Max',{40,5,10},'ScaleFactor',{1,1,1});

%%
% *Weights* 
%%
%%
% The control task is to get zero offset for piecewise-constant references,
% while avoiding instability due to input saturation.  Because both MV and
% OV variables are already scaled in MPC controller, MPC weights are
% dimensionless and applied to the scaled MV and OV values.  In this
% example, you penalize the two outputs equally with the same OV weights.
Weights = struct('MV',[1 1],'MVRate',[0.1 0.1],'OV',[0 0 1]);

%%
% *Construct the traditional MPC controller*
%%
% Create an MPC controller with plant model, sample time and horizons.
Ts = 0.05;          % Sampling time
p = 10;             % Prediction horizon
m = 2;              % Control horizon
mpcobj = mpc(plant,Ts,p,m,Weights,MV,OV);
mpcobj.Model.Plant=minreal(mpcobj.Model.Plant)
%% Generate Explicit MPC Controller
% Explicit MPC executes the equivalent explicit piecewise affine version of
% the MPC control law defined by the traditional MPC.  To generate an
% Explicit MPC from a traditional MPC, you must specify range for each
% controller state, reference signal, manipulated variable and measured
% disturbance so that the multi-parametric quadratic programming problem is
% solved in the parameter space defined by these ranges.
    
%% 
% *Obtain a range structure for initialization*
%%
% Use |generateExplicitRange| command to obtain a range structure where you
% can specify range for each parameter afterwards.
range = generateExplicitRange(mpcobj);

%% 
% *Specify ranges for controller states*
%%
% MPC controller states include states from plant model, disturbance model
% and noise model in that order.  Setting the range of a state variable is
% sometimes difficult when the state does not correspond to a physical
% parameter.  In that case, multiple runs of open-loop plant simulation
% with typical reference and disturbance signals are recommended in order
% to collect data that reflect the ranges of states.
range.State.Min(:) = -10000;
range.State.Max(:) = 10000;

%% 
% *Specify ranges for reference signals*
%%
% Usually you know the practical range of the reference signals being used
% at the nominal operating point in the plant.  The ranges used to generate
% Explicit MPC must be at least as large as the practical range.
range.Reference.Min = [0;-11;-8];
range.Reference.Max = [100;11;8];

%% 
% *Specify ranges for manipulated variables*
%%
% If manipulated variables are constrained, the ranges used to generate
% Explicit MPC must be at least as large as these limits.
range.ManipulatedVariable.Min = [MV(1).Min; MV(2).Min] - 1;
range.ManipulatedVariable.Max = [MV(1).Max; MV(2).Max] + 1;

%% 
% *Construct the Explicit MPC controller*
%%
% Use |generateExplicitMPC| command to obtain the Explicit MPC controller 
% with the parameter ranges previously specified.
mpcobjExplicit = generateExplicitMPC(mpcobj, range);
display(mpcobjExplicit);

%%
% Use |simplify| command with the "exact" method to join pairs of regions
% whose corresponding gains are the same and whose union is a convex set.
% This practice can reduce memory footprint of the Explicit MPC controller
% without sacrifice any performance.
mpcobjExplicitSimplified = simplify(mpcobjExplicit, 'exact');
display(mpcobjExplicitSimplified);

%%
% The number of piecewise affine region has been reduced.

%% Plot Piecewise Affine Partition
% You can review any 2-D section of the piecewise affine partition defined
% by the Explicit MPC control law.

%% 
% *Obtain a plot parameter structure for initialization*
%%
% Use |generatePlotParameters| command to obtain a parameter structure
% where you can specify which 2-D section to plot afterwards.
params = generatePlotParameters(mpcobjExplicitSimplified);

%% 
% *Specify parameters for a 2-D plot*
%%
% In this example, you plot the pitch angle (the 4th state variable) vs.
% its reference (the 2nd reference signal).  All the other parameters must
% be fixed at a value within its range.
%%
% Fix other state variables
params.State.Index = [1 2 3 5 6]; 
params.State.Value = [0 0 0 0 0];
%%
% Fix other reference signals
params.Reference.Index = 1;
params.Reference.Value = 0;
%%
% Fix manipulated variables
params.ManipulatedVariable.Index = [1 2];
params.ManipulatedVariable.Value = [0 0];

%% 
% *Plot the 2-D section*
%%
% Use |plotSection| command to plot the 2-D section defined previously.
plotSection(mpcobjExplicitSimplified, params);
axis([-10 10 -10 10]);
grid;
xlabel('Pitch angle (x_4)');
ylabel('Reference on pitch angle (r_2)');

%% Simulate Using Simulink(R)
% To run this example, Simulink(R) is required.
%if ~mpcchecktoolboxinstalled('simulink')
    disp('Simulink(R) is required to run this example.')
 %   return
%end
%%
% Simulate closed-loop control of the linear plant model in Simulink, using
% the Explicit MPC Controller block.  Controller "mpcobjExplicitSimplified"
% is specified in the block dialog.
mdl = 'empc_aircraft';
open_system(mdl)
sim(mdl)
%%
% The closed-loop response is identical to the traditional MPC controller
% designed in the "mpcaircraft" example.

%%
%bdclose(mdl)
