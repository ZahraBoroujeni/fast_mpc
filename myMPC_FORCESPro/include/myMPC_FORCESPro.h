/*
myMPC_FORCESPro : A fast customized optimization solver.

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

#ifndef __myMPC_FORCESPro_H__
#define __myMPC_FORCESPro_H__


/* DATA TYPE ------------------------------------------------------------*/
typedef double myMPC_FORCESPro_FLOAT;

typedef double myMPC_FORCESProINTERFACE_FLOAT;

/* SOLVER SETTINGS ------------------------------------------------------*/
/* print level */
#ifndef myMPC_FORCESPro_SET_PRINTLEVEL
#define myMPC_FORCESPro_SET_PRINTLEVEL    (2)
#endif

/* timing */
#ifndef myMPC_FORCESPro_SET_TIMING
#define myMPC_FORCESPro_SET_TIMING    (1)
#endif

/* Numeric Warnings */
/* #define PRINTNUMERICALWARNINGS */

/* maximum number of iterations  */
#define myMPC_FORCESPro_SET_MAXIT         (200)	

/* scaling factor of line search (affine direction) */
#define myMPC_FORCESPro_SET_LS_SCALE_AFF  (myMPC_FORCESPro_FLOAT)(0.9)      

/* scaling factor of line search (combined direction) */
#define myMPC_FORCESPro_SET_LS_SCALE      (myMPC_FORCESPro_FLOAT)(0.95)  

/* minimum required step size in each iteration */
#define myMPC_FORCESPro_SET_LS_MINSTEP    (myMPC_FORCESPro_FLOAT)(1E-08)

/* maximum step size (combined direction) */
#define myMPC_FORCESPro_SET_LS_MAXSTEP    (myMPC_FORCESPro_FLOAT)(0.995)

/* desired relative duality gap */
#define myMPC_FORCESPro_SET_ACC_RDGAP     (myMPC_FORCESPro_FLOAT)(0.0001)

/* desired maximum residual on equality constraints */
#define myMPC_FORCESPro_SET_ACC_RESEQ     (myMPC_FORCESPro_FLOAT)(1E-06)

/* desired maximum residual on inequality constraints */
#define myMPC_FORCESPro_SET_ACC_RESINEQ   (myMPC_FORCESPro_FLOAT)(1E-06)

/* desired maximum violation of complementarity */
#define myMPC_FORCESPro_SET_ACC_KKTCOMPL  (myMPC_FORCESPro_FLOAT)(1E-06)


/* RETURN CODES----------------------------------------------------------*/
/* solver has converged within desired accuracy */
#define myMPC_FORCESPro_OPTIMAL      (1)

/* maximum number of iterations has been reached */
#define myMPC_FORCESPro_MAXITREACHED (0)

/* no progress in line search possible */
#define myMPC_FORCESPro_NOPROGRESS   (-7)

/* fatal internal error - nans occurring */
#define myMPC_FORCESPro_NAN  (-10)


/* PARAMETERS -----------------------------------------------------------*/
/* fill this with data before calling the solver! */
typedef struct myMPC_FORCESPro_params
{
    /* vector of size 2 */
    myMPC_FORCESPro_FLOAT minusA_times_x0[2];

} myMPC_FORCESPro_params;


/* OUTPUTS --------------------------------------------------------------*/
/* the desired variables are put here by the solver */
typedef struct myMPC_FORCESPro_output
{
    /* vector of size 1 */
    myMPC_FORCESPro_FLOAT u0[1];

} myMPC_FORCESPro_output;


/* SOLVER INFO ----------------------------------------------------------*/
/* diagnostic data from last interior point step */
typedef struct myMPC_FORCESPro_info
{
    /* iteration number */
    int it;

	/* number of iterations needed to optimality (branch-and-bound) */
	int it2opt;
	
    /* inf-norm of equality constraint residuals */
    myMPC_FORCESPro_FLOAT res_eq;
	
    /* inf-norm of inequality constraint residuals */
    myMPC_FORCESPro_FLOAT res_ineq;

    /* primal objective */
    myMPC_FORCESPro_FLOAT pobj;	
	
    /* dual objective */
    myMPC_FORCESPro_FLOAT dobj;	

    /* duality gap := pobj - dobj */
    myMPC_FORCESPro_FLOAT dgap;		
	
    /* relative duality gap := |dgap / pobj | */
    myMPC_FORCESPro_FLOAT rdgap;		

    /* duality measure */
    myMPC_FORCESPro_FLOAT mu;

	/* duality measure (after affine step) */
    myMPC_FORCESPro_FLOAT mu_aff;
	
    /* centering parameter */
    myMPC_FORCESPro_FLOAT sigma;
	
    /* number of backtracking line search steps (affine direction) */
    int lsit_aff;
    
    /* number of backtracking line search steps (combined direction) */
    int lsit_cc;
    
    /* step size (affine direction) */
    myMPC_FORCESPro_FLOAT step_aff;
    
    /* step size (combined direction) */
    myMPC_FORCESPro_FLOAT step_cc;    

	/* solvertime */
	myMPC_FORCESPro_FLOAT solvetime;   

} myMPC_FORCESPro_info;









/* SOLVER FUNCTION DEFINITION -------------------------------------------*/
/* examine exitflag before using the result! */
#ifdef _cplusplus
extern "C" {
#endif
int myMPC_FORCESPro_solve(myMPC_FORCESPro_params* params, myMPC_FORCESPro_output* output, myMPC_FORCESPro_info* info, FILE* fs);

#ifdef _cplusplus
}
#endif

#endif