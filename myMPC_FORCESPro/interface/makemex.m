% myMPC_FORCESPro : A fast customized optimization solver.
% 
% Copyright (C) 2013-2016 EMBOTECH GMBH [info@embotech.com]. All rights reserved.
% 
% 
% This software is intended for simulation and testing purposes only. 
% Use of this software for any commercial purpose is prohibited.
% 
% This program is distributed in the hope that it will be useful.
% EMBOTECH makes NO WARRANTIES with respect to the use of the software 
% without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
% PARTICULAR PURPOSE. 
% 
% EMBOTECH shall not have any liability for any damage arising from the use
% of the software.
% 
% This Agreement shall exclusively be governed by and interpreted in 
% accordance with the laws of Switzerland, excluding its principles
% of conflict of laws. The Courts of Zurich-City shall have exclusive 
% jurisdiction in case of any dispute.
% 

if exist( '../src/myMPC_FORCESPro.c', 'file' )
    mex -c -O -DUSEMEXPRINTS ../src/myMPC_FORCESPro.c 
mex -c -O -DMEXARGMUENTCHECKS myMPC_FORCESPro_mex.c 
if( ispc )
    mex myMPC_FORCESPro.obj myMPC_FORCESPro_mex.obj -output "myMPC_FORCESPro" 
    delete('*.obj');
elseif( ismac )
    mex myMPC_FORCESPro.o myMPC_FORCESPro_mex.o -output "myMPC_FORCESPro"
    delete('*.o');
else % we're on a linux system
    mex myMPC_FORCESPro.o myMPC_FORCESPro_mex.o -output "myMPC_FORCESPro" -lrt
    delete('*.o');
end
copyfile(['myMPC_FORCESPro.',mexext], ['../../myMPC_FORCESPro.',mexext], 'f');
copyfile( 'myMPC_FORCESPro.m', '../../myMPC_FORCESPro.m','f');
else
    fprintf('Could not find source file. This file is meant to be used for building from source code.');
end
