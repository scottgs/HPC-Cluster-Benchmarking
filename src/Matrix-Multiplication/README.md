### Building ###
Make sure to have the required dependencies -- namely, PAPI and MPI. Then, simply run Make

### Running the program ###
The program accepts three arguments:
* argv[0] is the program name: namely, mm
* argv[1] is the number of rows for the matrix A
* argv[2] is the number of columns for the matrix A
* argv[3] is the number of columns for the matrix B
Note that the number of columns for matrix A and the number of rows for matrix B are assumed to be the same since the multiplication is not feasible otherwise.

### TO-DO ###
* Error check command line arguments and other places a bit more.
* Output results to file from PAPI timing statistics.
* Generate test script for scaling input sizes.
