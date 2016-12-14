/*
FORCESNLPsolver : A fast customized optimization solver.

Copyright (C) 2013-2016 EMBOTECH GMBH [info@embotech.com]. All rights reserved.


This software is intended for simulation and testing purposes only. 
Use of this software for any commercial purpose is prohibited.

This program is distributed in the hope that it will be useful.
EMBOTECH makes NO WARRANTIES with respect to the use of the software 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. 

EMBOTECH shall not have any liability for any damage arising from the use
of the software.

This Agreement shall exclusively be governed by and interpreted in 
accordance with the laws of Switzerland, excluding its principles
of conflict of laws. The Courts of Zurich-City shall have exclusive 
jurisdiction in case of any dispute.

*/

#include "mex.h"
#include "math.h"
#include "../include/FORCESNLPsolver.h"
#include <stdio.h>

/* copy functions */
void copyCArrayToM(double *src, double *dest, int dim) {
    while (dim--) {
        *dest++ = (double)*src++;
    }
}
void copyMArrayToC(double *src, double *dest, int dim) {
    while (dim--) {
        *dest++ = (double) (*src++) ;
    }
}


extern void FORCESNLPsolver_casadi2forces(double *x, double *y, double *l, double *p, double *f, double *nabla_f, double *c, double *nabla_c, double *h, double *nabla_h, double *H, int stage);
FORCESNLPsolver_ExtFunc pt2Function = &FORCESNLPsolver_casadi2forces;


/* Some memory for mex-function */
FORCESNLPsolver_params params;
FORCESNLPsolver_output output;
FORCESNLPsolver_info info;

/* THE mex-function */
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )  
{
	/* file pointer for printing */
	FILE *fp = NULL;

	/* define variables */	
	mxArray *par;
	mxArray *outvar;
	const mxArray *PARAMS = prhs[0];
	double *pvalue;
	int i;
	int exitflag;
	const char *fname;
	const char *outputnames[85] = {"x01","x02","x03","x04","x05","x06","x07","x08","x09","x10","x11","x12","x13","x14","x15","x16","x17","x18","x19","x20","x21","x22","x23","x24","x25","x26","x27","x28","x29","x30","x31","x32","x33","x34","x35","x36","x37","x38","x39","x40","x41","x42","x43","x44","x45","x46","x47","x48","x49","x50","x51","x52","x53","x54","x55","x56","x57","x58","x59","x60","x61","x62","x63","x64","x65","x66","x67","x68","x69","x70","x71","x72","x73","x74","x75","x76","x77","x78","x79","x80","x81","x82","x83","x84","x85"};
	const char *infofields[8] = { "it", "it2opt", "res_eq", "res_ineq",  "pobj",  "mu",  "solvetime",  "fevalstime"};
	
	/* Check for proper number of arguments */
    if (nrhs != 1) {
        mexErrMsgTxt("This function requires exactly 1 input: PARAMS struct.\nType 'help FORCESNLPsolver_mex' for details.");
    }    
	if (nlhs > 3) {
        mexErrMsgTxt("This function returns at most 3 outputs.\nType 'help FORCESNLPsolver_mex' for details.");
    }

	/* Check whether params is actually a structure */
	if( !mxIsStruct(PARAMS) ) {
		mexErrMsgTxt("PARAMS must be a structure.");
	}

	/* copy parameters into the right location */
	par = mxGetField(PARAMS, 0, "xinit");
#ifdef MEXARGMUENTCHECKS
    if( par == NULL )	{
        mexErrMsgTxt("PARAMS.xinit not found");
    }
    if( !mxIsDouble(par) )
    {
    mexErrMsgTxt("PARAMS.xinit must be a double.");
    }
    if( mxGetM(par) != 12 || mxGetN(par) != 1 ) {
    mexErrMsgTxt("PARAMS.xinit must be of size [12 x 1]");
    }
#endif	 
    copyMArrayToC(mxGetPr(par), params.xinit, 12);

	par = mxGetField(PARAMS, 0, "x0");
#ifdef MEXARGMUENTCHECKS
    if( par == NULL )	{
        mexErrMsgTxt("PARAMS.x0 not found");
    }
    if( !mxIsDouble(par) )
    {
    mexErrMsgTxt("PARAMS.x0 must be a double.");
    }
    if( mxGetM(par) != 1530 || mxGetN(par) != 1 ) {
    mexErrMsgTxt("PARAMS.x0 must be of size [1530 x 1]");
    }
#endif	 
    copyMArrayToC(mxGetPr(par), params.x0, 1530);

	par = mxGetField(PARAMS, 0, "all_parameters");
#ifdef MEXARGMUENTCHECKS
    if( par == NULL )	{
        mexErrMsgTxt("PARAMS.all_parameters not found");
    }
    if( !mxIsDouble(par) )
    {
    mexErrMsgTxt("PARAMS.all_parameters must be a double.");
    }
    if( mxGetM(par) != 170 || mxGetN(par) != 1 ) {
    mexErrMsgTxt("PARAMS.all_parameters must be of size [170 x 1]");
    }
#endif	 
    copyMArrayToC(mxGetPr(par), params.all_parameters, 170);

	#if FORCESNLPsolver_SET_PRINTLEVEL > 0
		/* Prepare file for printfs */
		/*fp = freopen("stdout_temp","w+",stdout);*/
        fp = fopen("stdout_temp","w+");
		if( fp == NULL ) {
			mexErrMsgTxt("freopen of stdout did not work.");
		}
		rewind(fp);
	#endif

	/* call solver */
	exitflag = FORCESNLPsolver_solve(&params, &output, &info, fp ,pt2Function);

	/* close stdout */
	/* fclose(fp); */
	
	#if FORCESNLPsolver_SET_PRINTLEVEL > 0
		/* Read contents of printfs printed to file */
		rewind(fp);
		while( (i = fgetc(fp)) != EOF ) {
			mexPrintf("%c",i);
		}
		fclose(fp);
	#endif

	/* copy output to matlab arrays */
	plhs[0] = mxCreateStructMatrix(1, 1, 85, outputnames);
	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x01, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x01", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x02, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x02", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x03, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x03", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x04, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x04", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x05, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x05", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x06, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x06", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x07, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x07", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x08, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x08", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x09, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x09", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x10, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x10", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x11, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x11", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x12, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x12", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x13, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x13", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x14, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x14", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x15, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x15", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x16, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x16", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x17, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x17", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x18, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x18", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x19, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x19", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x20, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x20", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x21, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x21", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x22, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x22", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x23, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x23", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x24, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x24", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x25, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x25", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x26, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x26", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x27, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x27", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x28, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x28", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x29, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x29", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x30, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x30", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x31, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x31", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x32, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x32", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x33, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x33", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x34, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x34", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x35, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x35", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x36, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x36", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x37, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x37", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x38, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x38", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x39, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x39", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x40, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x40", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x41, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x41", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x42, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x42", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x43, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x43", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x44, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x44", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x45, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x45", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x46, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x46", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x47, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x47", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x48, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x48", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x49, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x49", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x50, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x50", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x51, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x51", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x52, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x52", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x53, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x53", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x54, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x54", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x55, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x55", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x56, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x56", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x57, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x57", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x58, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x58", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x59, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x59", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x60, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x60", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x61, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x61", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x62, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x62", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x63, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x63", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x64, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x64", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x65, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x65", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x66, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x66", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x67, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x67", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x68, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x68", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x69, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x69", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x70, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x70", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x71, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x71", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x72, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x72", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x73, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x73", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x74, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x74", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x75, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x75", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x76, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x76", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x77, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x77", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x78, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x78", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x79, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x79", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x80, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x80", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x81, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x81", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x82, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x82", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x83, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x83", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x84, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x84", outvar);

	outvar = mxCreateDoubleMatrix(18, 1, mxREAL);
	copyCArrayToM( output.x85, mxGetPr(outvar), 18);
	mxSetField(plhs[0], 0, "x85", outvar);	

	/* copy exitflag */
	if( nlhs > 1 )
	{
		plhs[1] = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(plhs[1]) = (double)exitflag;
	}

	/* copy info struct */
	if( nlhs > 2 )
	{
		        plhs[2] = mxCreateStructMatrix(1, 1, 8, infofields);
         
		
		/* iterations */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = (double)info.it;
		mxSetField(plhs[2], 0, "it", outvar);

		/* iterations to optimality (branch and bound) */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = (double)info.it2opt;
		mxSetField(plhs[2], 0, "it2opt", outvar);
		
		/* res_eq */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = info.res_eq;
		mxSetField(plhs[2], 0, "res_eq", outvar);

		/* res_ineq */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = info.res_ineq;
		mxSetField(plhs[2], 0, "res_ineq", outvar);

		/* pobj */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = info.pobj;
		mxSetField(plhs[2], 0, "pobj", outvar);

		/* mu */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = info.mu;
		mxSetField(plhs[2], 0, "mu", outvar);

		/* solver time */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = info.solvetime;
		mxSetField(plhs[2], 0, "solvetime", outvar);

		/* solver time */
		outvar = mxCreateDoubleMatrix(1, 1, mxREAL);
		*mxGetPr(outvar) = info.fevalstime;
		mxSetField(plhs[2], 0, "fevalstime", outvar);
	}
}