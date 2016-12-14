/*  * CasADi to FORCES Template - missing information to be filled in by createCasadi.m 
 * (C) embotech GmbH, Zurich, Switzerland, 2013-16. All rights reserved.
 *
 * This file is part of the FORCES client, and carries the same license.
 */ 

#ifdef __cplusplus
extern "C" {
#endif
    
/* prototyes for models */
extern void FORCESNLPsolver_model_1(const double** arg, double** res);
extern void FORCESNLPsolver_model_1_sparsity(int i, int *nrow, int *ncol, const int **colind, const int **row);
extern void FORCESNLPsolver_model_85(const double** arg, double** res);
extern void FORCESNLPsolver_model_85_sparsity(int i, int *nrow, int *ncol, const int **colind, const int **row);
    

/* copies data from sparse matrix into a dense one */
void sparse2fullCopy(int nrow, int ncol, const int* colidx, const int* row, double *data, double *Out)
{
    int i, j;
    
    /* copy data into dense matrix */
    for(i=0; i<ncol; i++){
        for( j=colidx[i]; j < colidx[i+1]; j++ ){
            Out[i*nrow + row[j]] = data[j];
        }
    }
}

/* CasADi - FORCES interface */
void FORCESNLPsolver_casadi2forces(double *x,        /* primal vars                                         */
                   double *y,        /* eq. constraint multiplers                           */
                   double *l,        /* ineq. constraint multipliers                        */
                   double *p,        /* parameters                                          */
                   double *f,        /* objective function (scalar)                         */
                   double *nabla_f,  /* gradient of objective function                      */
                   double *c,        /* dynamics                                            */
                   double *nabla_c,  /* Jacobian of the dynamics (column major)             */
                   double *h,        /* inequality constraints                              */
                   double *nabla_h,  /* Jacobian of inequality constraints (column major)   */
                   double *H,        /* Hessian (column major)                              */
                   int stage         /* stage number (0 indexed)                            */
                  )
{
    /* CasADi input and output arrays */
    const double *in[4];
    double *out[7];
    
    /* temporary storage for casadi sparse output */
    double this_f;
    double nabla_f_sparse[8];
    double h_sparse[5];
    double nabla_h_sparse[16];
    double c_sparse[12];
    double nabla_c_sparse[42];
            
    
    /* pointers to row and column info for 
     * column compressed format used by CasADi */
    int nrow, ncol;
    const int *colind, *row;
    
    /* set inputs for CasADi */
    in[0] = x;
    in[1] = p; /* maybe should be made conditional */
    in[2] = l; /* maybe should be made conditional */     
    in[3] = y; /* maybe should be made conditional */
    
    /* set outputs for CasADi */
    out[0] = &this_f;
    out[1] = nabla_f_sparse;
                
	 if (stage >= 0 && stage < 84)
	 {
		 /* set inputs */
		 out[2] = h_sparse;
		 out[3] = nabla_h_sparse;
		 out[4] = c_sparse;
		 out[5] = nabla_c_sparse;
		 

		 /* call CasADi */
		 FORCESNLPsolver_model_1(in, out);

		 /* copy to dense */
		 if( nabla_f ){ FORCESNLPsolver_model_1_sparsity(3, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, nabla_f_sparse, nabla_f); }
		 if( c ){ FORCESNLPsolver_model_1_sparsity(6, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, c_sparse, c); }
		 if( nabla_c ){ FORCESNLPsolver_model_1_sparsity(7, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, nabla_c_sparse, nabla_c); }
		 if( h ){ FORCESNLPsolver_model_1_sparsity(4, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, h_sparse, h); }
		 if( nabla_h ){ FORCESNLPsolver_model_1_sparsity(5, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, nabla_h_sparse, nabla_h); }
		 
	 }

	 if (stage >= 84 && stage < 85)
	 {
		 /* set inputs */
		 out[2] = h_sparse;
		 out[3] = nabla_h_sparse;
		 /* call CasADi */
		 FORCESNLPsolver_model_85(in, out);

		 /* copy to dense */
		 if( nabla_f ){ FORCESNLPsolver_model_85_sparsity(3, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, nabla_f_sparse, nabla_f); }
		 if( h ){ FORCESNLPsolver_model_85_sparsity(4, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, h_sparse, h); }
		 if( nabla_h ){ FORCESNLPsolver_model_85_sparsity(5, &nrow, &ncol, &colind, &row); sparse2fullCopy(nrow, ncol, colind, row, nabla_h_sparse, nabla_h); }
		 
	 }

         
    
    /* add to objective */
    if( f ){
        *f += this_f;
    }
}

#ifdef __cplusplus
} /* extern "C" */
#endif