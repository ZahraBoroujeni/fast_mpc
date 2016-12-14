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

#include <stdio.h>

#ifndef __FORCESNLPsolver_H__
#define __FORCESNLPsolver_H__


/* DATA TYPE ------------------------------------------------------------*/
typedef double FORCESNLPsolver_FLOAT;

typedef double FORCESNLPsolverINTERFACE_FLOAT;

/* SOLVER SETTINGS ------------------------------------------------------*/
/* print level */
#ifndef FORCESNLPsolver_SET_PRINTLEVEL
#define FORCESNLPsolver_SET_PRINTLEVEL    (2)
#endif

/* timing */
#ifndef FORCESNLPsolver_SET_TIMING
#define FORCESNLPsolver_SET_TIMING    (1)
#endif

/* Numeric Warnings */
/* #define PRINTNUMERICALWARNINGS */

/* maximum number of iterations  */
#define FORCESNLPsolver_SET_MAXIT			(3000)	

/* scaling factor of line search (FTB rule) */
#define FORCESNLPsolver_SET_FLS_SCALE		(FORCESNLPsolver_FLOAT)(0.99)      

/* maximum number of supported elements in the filter */
#define FORCESNLPsolver_MAX_FILTER_SIZE	(3000) 

/* maximum number of supported elements in the filter */
#define FORCESNLPsolver_MAX_SOC_IT			(4) 

/* desired relative duality gap */
#define FORCESNLPsolver_SET_ACC_RDGAP		(FORCESNLPsolver_FLOAT)(0.0001)

/* desired maximum residual on equality constraints */
#define FORCESNLPsolver_SET_ACC_RESEQ		(FORCESNLPsolver_FLOAT)(1E-06)

/* desired maximum residual on inequality constraints */
#define FORCESNLPsolver_SET_ACC_RESINEQ	(FORCESNLPsolver_FLOAT)(1E-06)

/* desired maximum violation of complementarity */
#define FORCESNLPsolver_SET_ACC_KKTCOMPL	(FORCESNLPsolver_FLOAT)(1E-06)


/* RETURN CODES----------------------------------------------------------*/
/* solver has converged within desired accuracy */
#define FORCESNLPsolver_OPTIMAL      (1)

/* maximum number of iterations has been reached */
#define FORCESNLPsolver_MAXITREACHED (0)

/* NaN encountered in function evaluations */
#define FORCESNLPsolver_BADFUNCEVAL  (-6)

/* no progress in method possible */
#define FORCESNLPsolver_NOPROGRESS   (-7)


/* Structure for defining the filter */
typedef struct FILTER
{
    /* comparison values for theta >= */
    double theta_comp[FORCESNLPsolver_MAX_FILTER_SIZE];

    /* comparison values for psi >= */
    double psi_comp[FORCESNLPsolver_MAX_FILTER_SIZE];

	/* current number of elements in filter */
	int no_elements;

} FILTER;

/* PARAMETERS -----------------------------------------------------------*/
/* fill this with data before calling the solver! */
typedef struct FORCESNLPsolver_params
{
    /* vector of size 12 */
    FORCESNLPsolver_FLOAT xinit[12];

    /* vector of size 1530 */
    FORCESNLPsolver_FLOAT x0[1530];

    /* vector of size 170 */
    FORCESNLPsolver_FLOAT all_parameters[170];

} FORCESNLPsolver_params;


/* OUTPUTS --------------------------------------------------------------*/
/* the desired variables are put here by the solver */
typedef struct FORCESNLPsolver_output
{
    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x01[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x02[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x03[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x04[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x05[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x06[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x07[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x08[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x09[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x10[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x11[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x12[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x13[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x14[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x15[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x16[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x17[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x18[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x19[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x20[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x21[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x22[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x23[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x24[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x25[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x26[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x27[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x28[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x29[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x30[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x31[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x32[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x33[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x34[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x35[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x36[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x37[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x38[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x39[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x40[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x41[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x42[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x43[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x44[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x45[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x46[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x47[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x48[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x49[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x50[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x51[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x52[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x53[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x54[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x55[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x56[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x57[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x58[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x59[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x60[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x61[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x62[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x63[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x64[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x65[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x66[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x67[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x68[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x69[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x70[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x71[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x72[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x73[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x74[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x75[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x76[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x77[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x78[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x79[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x80[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x81[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x82[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x83[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x84[18];

    /* vector of size 18 */
    FORCESNLPsolver_FLOAT x85[18];

} FORCESNLPsolver_output;


/* SOLVER INFO ----------------------------------------------------------*/
/* diagnostic data from last interior point step */
typedef struct FORCESNLPsolver_info
{
    /* iteration number */
    int it;

	/* number of iterations needed to optimality (branch-and-bound) */
	int it2opt;
	
    /* inf-norm of equality constraint residuals */
    FORCESNLPsolver_FLOAT res_eq;
	
    /* inf-norm of inequality constraint residuals */
    FORCESNLPsolver_FLOAT res_ineq;

    /* primal objective */
    FORCESNLPsolver_FLOAT pobj;	
	
    /* dual objective */
    FORCESNLPsolver_FLOAT dobj;	

    /* duality gap := pobj - dobj */
    FORCESNLPsolver_FLOAT dgap;		
	
    /* relative duality gap := |dgap / pobj | */
    FORCESNLPsolver_FLOAT rdgap;		

    /* duality measure */
    FORCESNLPsolver_FLOAT mu;

	/* duality measure (after affine step) */
    FORCESNLPsolver_FLOAT mu_aff;
	
    /* centering parameter */
    FORCESNLPsolver_FLOAT sigma;
	
    /* number of backtracking line search steps (affine direction) */
    int lsit_aff;
    
    /* number of backtracking line search steps (combined direction) */
    int lsit_cc;
    
    /* step size (affine direction) */
    FORCESNLPsolver_FLOAT step_aff;
    
    /* step size (combined direction) */
    FORCESNLPsolver_FLOAT step_cc;    

	/* solvertime */
	FORCESNLPsolver_FLOAT solvetime;   

	/* time spent in function evaluations */
	FORCESNLPsolver_FLOAT fevalstime;  

} FORCESNLPsolver_info;







typedef void (*FORCESNLPsolver_ExtFunc)(FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, FORCESNLPsolver_FLOAT*, int);

/* SOLVER FUNCTION DEFINITION -------------------------------------------*/
/* examine exitflag before using the result! */
#ifdef _cplusplus
extern "C" {
#endif																																
int FORCESNLPsolver_solve(FORCESNLPsolver_params* params, FORCESNLPsolver_output* output, FORCESNLPsolver_info* info, FILE* fs, FORCESNLPsolver_ExtFunc FORCESNLPsolver_evalExtFunctions);	

#ifdef _cplusplus
}
#endif

#endif