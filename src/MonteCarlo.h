/*
 * Author: Muhammad Haseeb
 * Class: CS6260 | Spring-2018
 * File: MonteCarlo.h
 * Assignment: MonteCarlo's Method
 * Date: 02/08/2018
 *
 */

#ifndef MONTECARLO_H_
#define MONTECARLO_H_

/* Include STD files */
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>

/* Include MPI */
#include <mpi.h>

/* Enable if debug info required - Works with Windows only */
#undef DEBUG
#undef WINDOWS

/* Error codes */
#define ERR_PROC_ID           -1
#define ERR_INVLD_ARGS        -2

/* Defines */
#define MASTER_THREAD          0
#define DEFAULT_TRIALS         100000000
#define TRIALS_TAG             1<<1
#define SUMS_TAG               1<<2

/* Macro to avoid compiler warnings if a param is not being used */
#define MC_UNUSED_PARAM(arg)   (void) arg

/* Typedefines */
typedef unsigned int   UINT;
typedef unsigned char  UCHAR;
typedef unsigned short USHORT;
typedef int            STATUS;


/* Function Prototypes */
STATUS main(int argc, char** argv);
UINT MonteCarlo(UINT num_trials, int proc_id);
double Random_Number();


#endif /* MONTECARLO_H_ */
