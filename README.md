# HPC Cluster Benchmarking #
Developing high performance, parallel and distributed algorithms for benchmarking a supercomputer (cluster with 27 nodes and growing) using the [PAPI High Performance Computing Framework](http://icl.cs.utk.edu/papi/overview/). This will serve as examples of programs I use to benchmark the supercomputer and experiment with different distributed computing paradigms. Eventually this will turn into a small library for testing clusters with HPC algorithms using these different paradigms and libraries.

## Dependencies ##
The major dependencies include PAPI and MPI.  For your ease, simply run the PAPI shell script located in the parent directory to download PAPI. 
To run the shell script, execute the following command: ./papi.sh

## Installing MPI ##
For instructions on how to install MPI and set your PATH and LD_LIBRARY_PATH variables, check out [this](http://www.itp.phys.ethz.ch/education/hs12/programming_techniques/openmpi.pdf).

## How to compile and run MPI programs ##
To compile your MPI C++ programs you have to use mpicxx with the same arguments as you would for g++. To run an MPI program called PROGRAM_NAME with N MPI processes, you would use the following command: mpirun -np N PROGRAM_NAME.

## Code ##
The source contains the following work:

* Parallel Matrix Multiplication using MPI (C)
* Bitonic Sort using Boost MPI (C++) -- in progress

## Building (CLI) ##
* build libraries in dependencies folder (./clean.sh, ./papi.sh)

## TO-DO ##
* Create a bash script to execute the Makefiles and test scripts for each algorithm in the src/ folder. Alternative, may use config file and let others pick and choose which algorithms to benchmark.
