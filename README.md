# mpi_pi_estimate
MPI based Monte Carlo Algorithm to estimate value of Pi

# HOW TO COMPILE:
 
 $ make
 
 MonteCarlo.exe will be created at the root directory of this folder */
  
## How to Compile on Windows
 
 1. You will need libmsmpi.
 2. Follow this link to setup http://www.math.ucla.edu/~wotaoyin/windows_coding.html
 3. Once done, just use the $make command.

# HOW TO RUN:
 
 $ mpiexec -n <Number of Processes> MonteCarlo.exe <number of tosses>
