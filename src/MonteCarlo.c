/*
 * Author: Muhammad Haseeb
 * Class: CS6260 | Spring-2018
 * File: MonteCarlo.h
 * Assignment: MonteCarlo's Method
 * Date: 02/08/2018
 *
 */

/* Include file */
#include "MonteCarlo.h"

#ifdef DEBUG
 #ifdef WINDOWS
  #include <Windows.h>
 #endif /* WINDOWS */
#endif /* DEBUG */

/*
 * FUNCTION: main
 * DESCRIPTION: Initializes the MPI, runs the
                Monte Carlo's Algorithm and
				finalizes the MPI.
 *
 * @params: argc, argv
 * @returns: status
 */
STATUS main(int argc, char** argv)
{
    STATUS status = MPI_SUCCESS;
    /* MPI variables */
    int proc_id = ERR_PROC_ID;
    int num_procs_fix = 0;
    double start_time = 0;
    double end_time = 0;
    MPI_Comm comm = MPI_COMM_WORLD;

    /*Number of trials */
    UINT num_trials = 0;
    UINT num_trials_loc = 0;
    int num_procs = 0;

	/* Inside Circle */
    UINT inside_circle = 0;
    UINT recv = 0;

    /* Value of pi */
    double pi = 0.0;

    /* Handle unused parameters to avoid compiler warnings */
    MC_UNUSED_PARAM(argc);
    MC_UNUSED_PARAM(argv);

    /* Initialize MPI */
    status = MPI_Init(&argc, &argv);

    // Get the number of processes
    if (status == MPI_SUCCESS)
    {
        status = MPI_Comm_size(comm, &num_procs);

        if (status == MPI_SUCCESS)
        {
            // Get the rank of the process
            status = MPI_Comm_rank(comm, &proc_id);
        }
    }

    /* Check if number of tosses passed as parameter */
    if (argc < 2)
    {
        if (proc_id == MASTER_THREAD)
        {
            printf("ERROR: Number of tosses not specified\n");
            printf("USAGE: mpiexec -n <PROCESSES> ./your_executable <NUMBER_OF_TOSSES>\n");
        }

        status = ERR_INVLD_ARGS;
    }

    /* Convert the number of tosses into UINT and broadcast to all the processes */
    if (status == MPI_SUCCESS)
    {
        if (proc_id == MASTER_THREAD)
        {
            num_trials = atoi(argv[1]);

            if (num_trials <= 0)
            {
                num_trials = DEFAULT_TRIALS;

                /* Use the default number of tosses if an invalid number entered */
                if (proc_id == MASTER_THREAD)
                {
                    printf("ERROR: Invalid Number Entered = %d\n", num_trials);
                    printf("Using Default Number of Tosses = %d\n", DEFAULT_TRIALS);
                }
            }
        }

        status = MPI_Bcast(&num_trials, 1, MPI_UNSIGNED, MASTER_THREAD, comm);

    }

    if (status == MPI_SUCCESS)
    {
        num_procs_fix = num_procs;

        /* Get the local number of trials */
        if (num_trials % num_procs != 0)
        {
            num_trials_loc = 1;
            num_trials_loc += num_trials/num_procs;
            num_trials = num_trials_loc * num_procs;
        }
        else
        {
            num_trials_loc = num_trials/num_procs;
        }

        /* Sync all processes here */
        MPI_Barrier(comm);

        start_time = MPI_Wtime();

        /* Monte Carlo Method */
        inside_circle = MonteCarlo(num_trials_loc, proc_id);
    }

#ifdef DEBUG
    cout << "Inside Circles = " << inside_circle << endl;
#endif /* DEBUG */

    /* Tree to join back */
    UINT tree_id = proc_id;
    int delta = 1;

    if (num_procs % 2 != 0)
    {
        tree_id ++;
    }

    if (proc_id != MASTER_THREAD && status == MPI_SUCCESS)
    {
        while (num_procs > 1)
        {
            if (tree_id % 2 != 0)
            {
                int dst = ((proc_id - delta) >= 0) ? (proc_id - delta) : 0;
#ifdef DEBUG
                cout << "SEND: proc num = " << proc_id << "  tree id = " << tree_id << " Dest = " << dst << endl;
#endif /* DEBUG */
                MPI_Send((void *) &inside_circle,
                         1,
                         MPI_INT,
                         dst,
                         SUMS_TAG,
                         comm);
                break;
            }
            else
            {
                int src = (proc_id + delta); //< num_procs ? proc_id + delta : num_procs - 1;
#ifdef DEBUG
                cout << "RECEIVE: proc num = " << proc_id << "  tree id = " << tree_id << " Source = " << src << endl;
#endif /* DEBUG */
                MPI_Recv((void *) &recv,
                         1,
                         MPI_INT,
                         src,
                         SUMS_TAG,
                         comm,
                         MPI_STATUS_IGNORE);

                inside_circle += recv;

                /* Update delta */
                delta *= 2;

                /* Update remaining number of processes */
                if (num_procs % 2 == 0)
                {
                    num_procs /= 2;
                }
                else
                {
                    num_procs = num_procs / 2 + 1;
                }

                /* Update tree id */
                if (num_procs %2 == 0)
                {
                    tree_id /= 2;
                }
                else
                {
                    tree_id = (tree_id / 2) + 1;
                }
            }
        }
    }

    if (proc_id == MASTER_THREAD && status == MPI_SUCCESS)
    {
        while (num_procs > 1)
        {
            /* Routine for Master process */
            if (num_procs % 2 == 0)
            {
#ifdef DEBUG
                cout << "RECEIVE: proc num = " << proc_id << endl;
#endif /* DEBUG */
                MPI_Recv((void *) &recv,
                         1,
                         MPI_INT,
                         MPI_ANY_SOURCE,
                         SUMS_TAG,
                         comm,
                         MPI_STATUS_IGNORE);

                inside_circle += recv;
                num_procs /= 2;
            }
            else
            {
                num_procs = (num_procs / 2) + 1;
            }
        }

#ifdef DEBUG
        cout << "Proc 0 has the sum now = " << inside_circle <<endl;
#endif /* DEBUG */

        /* Value of pi here */
        pi = 4.0 * inside_circle /(double)num_trials;

        /* TODO: Add hook here */
        end_time = MPI_Wtime();
		
        printf("\nNumber of processes = %d\n", num_procs_fix);
        printf("Number of tosses = %u\n", num_trials);
        printf("Estimated value of PI = %0.20f\n", pi);
#ifdef DEBUG
        Sleep(5000);
#endif
        printf("Start Time = %0.6fs\n", start_time);
        printf("End Time = %0.6fs\n", end_time);

        printf("Time Elapsed = %0.5f s\n\n", ((double)end_time - (double)start_time));
    }

    /* Finalize the MPI environment */
    status = MPI_Finalize();

    return status;
}

/*
 * FUNCTION: MonteCarlo
 * DESCRIPTION: Implementation of Monte Carlo's
                Algorithm
 *
 * @params: number of trials, process id
 * @returns: number of tosses inside circle
 */
UINT MonteCarlo(UINT num_trials, int proc_id)
{
    double x = 0;
    double y = 0;
    double distance = 0;
    UINT icircle = 0;

    /* Provide a random seed */
    srand((int)(time(0) * 23) % (proc_id + 5) + (proc_id + 1) * 29 - time(0) + (int)time(0) % (proc_id + 1) * time(0));

    for (UINT i = 0; i < num_trials; i++)
    {
        x  = Random_Number();
        x -= 1.0;
        y  = Random_Number();
        y -= 1.0;

        distance = x*x + y*y;

        if (distance <= 1)
        {
            icircle++;
        }
    }

    return icircle;
}

/*
 * FUNCTION: Random Number
 * DESCRIPTION: Returns a random number between
                -1.0 and 1.0
 *
 * @params: none
 * @returns: random number [-1.0, 1.0]
 */
double Random_Number()
{
    double val = 0;

    val = rand()/(double)(RAND_MAX/2);

    return val;
}
