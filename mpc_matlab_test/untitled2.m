A = [1 0 0 0 0 0 0; 0 1 0 0 0 0 0; 0 0.1 1 0 0 0 0; 0 0 0 1 0 0 0; 0 0 0 0.1 1 0 0; 0 0 0 0 0 1 0; 0 0 0 0 0 0.1 1];
B=[1 0; 0 1; 0 0;1 0; 0 0; 0 1;0 0];
C = [1 0 0 0 0 0 0; 0 1 0 0 0 0 0; 0 0 1 0 0 0 0; 0 0 0 1 0 0 0;0 0 0 0 1 0 0;0 0 0 0 0 1 0;0 0 0 0 0 0 1];
D = zeros(7,2);
plant = ss(A,B,C,D)
x0 = zeros(7,1);

%%
% The manipulated variables are the elevator and flaperon angles, the
% attack and pitch angles are measured outputs to be regulated. 
%
% The open-loop response of the system is unstable.
pole(plant)

plant.InputName = {'ax','ay'};
plant.OutputName = {'vx','vy','y','vrx','xr','vyr','yr'};
plant.StateName = {'vx','vy','y','vrx','xr','vyr','yr'};
plant.InputGroup.MV = 2;
%plant.InputGroup.MD = 4;
plant.OutputGroup.MO = 7;
%plant.OutputGroup.UO = 1;

old_status = mpcverbosity('off');
Ts = 0.1;
MPCobj = mpc(plant,Ts);
get(MPCobj)
mpcprops;
MPCobj.PredictionHorizon = 100;
MPCobj.Model.Plant.OutputUnit = {'m/s','m/s','m','m/s','m','m/s','m'};
MPCobj.Model.Plant.InputUnit = {'m/s^2','m/s^2'};


%display(MPCobj)
MPCobj.OV(1).Max = 100 ;
MPCobj.OV(2).Max = 20 ;
MPCobj.OV(3).Max = 8 ;
MPCobj.OV(1).Min = 0 ;
MPCobj.OV(2).Min = 0 ;
MPCobj.OV(3).Min = -8 ;
MPCobj.MV(1).Max = 4;
MPCobj.MV(2).Max = 4;
MPCobj.MV(1).Min = -4;
MPCobj.MV(2).Min = -4;

T = 50;
r =  [100 0.1 4 0 0 0 0];
%MPCobj.Model.Nominal.X= [100 0 4]

MPCobj.Model.Plant=minreal(MPCobj.Model.Plant)
%MPCobj.Model.Nominal.X= [100 0 4]

sim(MPCobj,T,r)
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
range = generateExplicitRange(MPCobj);

%% 
% *Specify ranges for controller states*
%%
% MPC controller states include states from plant model, disturbance model
% and noise model in that order.  Setting the range of a state variable is
% sometimes difficult when the state does not correspond to a physical
% parameter.  In that case, multiple runs of open-loop plant simulation
% with typical reference and disturbance signals are recommended in order
% to collect data that reflect the ranges of states.
range.State.Min(:) = -10;
range.State.Max(:) = 10;

%% 
% *Specify ranges for reference signals*
%%
% Usually you know the practical range of the reference signals being used
% at the nominal operating point in the plant.  The ranges used to generate
% Explicit MPC must be at least as large as the practical range.
range.Reference.Min = [90;-1;-4;-1;-1;-1;-1];
range.Reference.Max = [200;1;4;1;1;1;1];

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
mpcobjExplicit = generateExplicitMPC(MPCobj, range);
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
if ~mpcchecktoolboxinstalled('simulink')
    disp('Simulink(R) is required to run this example.')
    return
end
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
bdclose(mdl)

