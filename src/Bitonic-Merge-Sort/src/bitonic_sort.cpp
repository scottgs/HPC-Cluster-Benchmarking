#include <cmath>
#include <iostream> 
#include <mpi.h>

#include <boost/lexical_cast.hpp>

#include "../include/bitonic_sort.h"

int main(int argc, char ** argv) {
	// Initialize the MPI Environment and usual jazz
	MPI_Init(&argc, &argv);

	// Rank for each process and the total number of processes
	int procRank, nProcs;
	MPI_Comm_size(MPI_COMM_WORLD, &nProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

	// Size of list is determined by user
	const uint listSize = boost::lexical_cast<unsigned int>(argv[1]);	
	long long array[listSize];

	// Generate some random data to play with.

	return 0;
}