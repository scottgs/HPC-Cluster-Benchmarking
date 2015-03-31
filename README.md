# HPC-Distributed-Algorithms #
Developing high performance, parallel and distributed algorithms for benchmarking a supercomputer (cluster with 27 nodes and growing). This will serve as examples of programs I use to benchmark the supercomputer and experiment with different distributed computing paradigms. Eventually this will turn into a small library for testing clusters with HPC algorithms using different.

## DEPENDENCIES ##
The major dependencies include PAPI and MPI.  For your ease, simply run the PAPI shell script located in the parent directory to download PAPI. 
To run the shell script, execute the following command: ./papi.sh

## INSTALLING MPI ##
For instructions on how to install MPI and set your PATH and LD_LIBRARY_PATH variables, visit: http://www.itp.phys.ethz.ch/education/hs12/programming_techniques/openmpi.pdf

## HOW TO COMPILE AND RUN MPI PROGRAMS ##
To compile your MPI C++ programs you have to use mpicxx with the same arguments as you would for g++. To run an MPI program called PROGRAM_NAME with N MPI processes, you would use the following command: mpirun -np N PROGRAM_NAME.

## Code ##
The source contains the following work:

* Parallel Matrix Multiplication using MPI (C)
* Bitonic Sort using Boost MPI (C++) -- in progress

## BUILDING (CLI) ##
* build libraries in dependencies folder (./clean.sh, ./papi.sh)
