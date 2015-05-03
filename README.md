# HPC Cluster Benchmarking #
Developing high performance, parallel and distributed algorithms for benchmarking a supercomputer (cluster with 27 nodes and growing) using the [PAPI High Performance Computing Framework](http://icl.cs.utk.edu/papi/overview/). This will serve as examples of programs I use to benchmark the supercomputer and experiment with different distributed computing paradigms. Eventually this will turn into a small library for testing clusters with HPC algorithms using these different paradigms and libraries.

## Dependencies ##
The major dependencies include PAPI, Boost and MPI.  For your ease, simply run the shell scripts located in the tools folder to install the needed dependencies.

## Installing MPI ##
For instructions on how to install MPI and set your PATH and LD_LIBRARY_PATH variables, check out [this](http://www.itp.phys.ethz.ch/education/hs12/programming_techniques/openmpi.pdf).

## How to compile and run MPI programs ##
To compile your MPI C++ programs you have to use mpicxx with the same arguments as you would for g++. To run a program with N MPI processes, you would use the following command: mpirun -np N /path/to/binary/of/program program_arguments_go_here.

## Code ##
The source contains the following work in which I have used PAPI to track various statistics:
* Parallel Matrix Multiplication using MPI (C)
* Bitonic Sort using MPI (C++) -- Simple one works fine, more complicated MPI still has some bugs

## Examples ##
Since the documentation regarding PAPI usage is not great, I have compiled some *simple* standalone examples for getting introduced to PAPI. For lengthier uses and actual use in source code, check out the src folder. 

## Building (CLI) ##
* Install the dependencies mentioned above using the shell scripts located in the tools folder.
* Run the Makefiles for the respective algorithms you wish to benchmark with and then run the respective program(s).

## Initial Probing of System to Prepare for PAPI Use ##
* After installing PAPI, run the command: papi_avail -e PAPI_TOT_INS to show the available events and hardware information for use to you.

## TO-DO ##
* Create a bash script to execute the Makefiles and test scripts for each algorithm in the src/ folder. Alternative, may use config file and let others pick and choose which algorithms to benchmark.
