### Building ###
Make sure to have the required dependencies -- namely, PAPI and MPI. Then, simply run Make

### Running the program ###
The program accepts four arguments:
* argv[0] is the program name: namely, mm
* argv[1] is the number of rows for the matrix A
* argv[2] is the number of columns for the matrix A
* argv[3] is the number of columns for the matrix B

Note that the number of columns for matrix A and the number of rows for matrix B are assumed to be the same since the multiplication is not feasible otherwise.

```
mpirun -np <number of processors you wish to use> <executable> [program arguments]
```

### TO-DO ###
* Output results to file from PAPI timing statistics.
* Generate test script for scaling input sizes.
