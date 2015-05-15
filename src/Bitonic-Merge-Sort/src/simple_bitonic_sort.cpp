// #include "../include/simple_bitonic_sort.h"

// #define MASTER_NODE 0

// using namespace std;

// template< typename T >
// inline void generate_list_data(vector<T> & vec) {
// 	std::random_device rd;
//     std::mt19937 mt(rd());
//     std::uniform_real_distribution<double> dist(1, 100);

//     for (size_t i = 0; i < vec.size(); ++i) {
//     	vec[i] = dist(mt);
//     }
// }

// #ifdef NUMS_DEBUG
// template< typename T >
// inline void print_list(const vector<T> & vec) {
// 	int rank;
// 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
// 	cout << "Format is array[processRank][index] for printing purposes." << endl;
// 	for (size_t i = 0; i < vec.size(); ++i) {
// 		cout << "Vector[" << rank << "][" << i << "] = " << vec[i] << endl;
// 	}
// }
// #endif

// template < typename T >
// T path_and_check(int argc, char ** argv) {
// 	if (argc < 2) {
// 		cerr << "Not enough parameters!" << endl;
// 		cerr << "To run the program, use mpirun -np NUM_PROCESSORS path/to/binary NUM_ITEMS_TO_SORT." << endl; 
// 		_exit(EXIT_FAILURE);
// 	}
	
// 	T listSize;
// 	try {
// 		listSize = boost::lexical_cast<unsigned long>(argv[1]);
// 	}
// 	catch(boost::bad_lexical_cast & e) {
// 		cerr << "Number of elements to sort must be an integer > 0." << endl;
// 		_exit(EXIT_FAILURE);
// 	}	
// 	return listSize;
// }

// template < typename T >
// void bitonic_sort(vector<T> & nums, const int numProcs, const int rank) {
// 	const size_t maxIterations = log2(numProcs);
// 	int swapPartner = 0; // Process that will do binary comparisons and swaps each step
// 	vector<T> sortBuffer(2 * nums.size()); // For the merging step

// 	bool windowIDEven = false;
// 	bool jBitZero = false; // If jth bit of procRank is zero

// 	MPI_Status status;
// 	/* Be careful about your for loops using ints as the type and not size_t
// 	 * since doing bitwise operations on them only makes sense in the context of 
// 	 * integers.
// 	 */
// 	for (int i = 1; i <= maxIterations; ++i) {
// 		/* Calculate the windowID and determine if even. */
// 		windowIDEven = ((rank >> i) & 1) ? false : true;
// 		for (int j = (i-1); j >= 0; --j) {
// 			/* Calculate communication partner by XOR with 1 shifted left j bits. */
// 			swapPartner = rank ^ (1 << j);

// 			/* Determine if jth bit of particular process's rank is 0. */
// 			jBitZero = (rank & (1 << j)) ? false : true;

// 			/* Compare Exchange LOW */
// 			if ((windowIDEven && jBitZero) || (!windowIDEven && !jBitZero)) {
// 				/* Send the entire vector as one block to swapPartner. */
// 				MPI_Send(
// 					&nums[0],			// Init address of data
// 					nums.size(),		// Count
// 					MPI_INT,			// Send datatype
// 					swapPartner,		// Destination process
// 					0,					// Tag
// 					MPI_COMM_WORLD		// Communicator
// 				);
// 				/* After the numbers have been edited and sorted, receive them. */
// 				MPI_Recv(
// 					&nums[0],			// Init address of receive buffer
// 					nums.size(),		// Count
// 					MPI_INT,			// Receive datatype
// 					swapPartner,		// Source process
// 					0,					// Tag
// 					MPI_COMM_WORLD,		// Communicator
// 					&status 			// Status
// 				);
// 			}
// 			/* Compare Exchange HIGH */
// 			else {
// 				/* Receive LOW's entire vector in one block */
// 				MPI_Recv(
// 					&sortBuffer[0],		// Init address of receive buffer
// 					nums.size(),		// Count
// 					MPI_INT,			// Receive datatype
// 					swapPartner,		// Source process
// 					0,					// Tag
// 					MPI_COMM_WORLD,		// Communicator
// 					&status 			// Status
// 				);

// 				copy(nums.begin(), nums.end(), sortBuffer.begin() + nums.size());

// 				/* Instead of doing my own sort, just use sort from STL.
// 				 * TODO: Can definitely improve with this
// 				 */
// 				sort(sortBuffer.begin(), sortBuffer.end());

// 				nums.assign(sortBuffer.begin() + nums.size(), sortBuffer.end());

// 				/* Send LOW the lower half of the vector. */
// 				MPI_Send(
// 					&sortBuffer[0],		// Init address of data
// 					nums.size(),		// Count
// 					MPI_INT,			// Send datatype
// 					swapPartner,		// Destination process
// 					0,					// Tag
// 					MPI_COMM_WORLD		// Communicator
// 				);
// 			}	
// 		}
// 	}
// }

// int main(int argc, char ** argv) {
// 	/* Init the MPI environment. */
// 	int procID, numProcs;
// 	MPI_Init(&argc, &argv);
// 	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
// 	MPI_Comm_rank(MPI_COMM_WORLD, &procID);

// 	const unsigned long listSize = path_and_check<unsigned long>(argc, argv);
// 	vector<int> nums(listSize);

// 	/* Generate some random data to sort. */
// 	srand(static_cast<unsigned int>(time(NULL)));
// 	generate_list_data<int>(nums);

// 	#ifdef NUMS_DEBUG
// 		print_list(nums);
// 	#endif

// 	double wallClockStart = 0, wallClockEnd = 0;

// 	if (procID == MASTER_NODE) {
// 		cout << "Number of processes being used is: " << numProcs << endl;
// 		cout << "Number of numbers to sort per process is: " << listSize << endl;
// 		wallClockStart = MPI_Wtime();
// 	}

// 	bitonic_sort<int>(nums, numProcs, procID);

// 	/* Wait for all processes to finish bitonic sorting. */
//   	MPI_Barrier(MPI_COMM_WORLD);

//   	/* Stop the clock. The time is marked BEFORE we call is_sorted for sanity check. */
//   	if (procID == MASTER_NODE) {
//   		wallClockEnd = MPI_Wtime();
//   		auto diff = wallClockEnd - wallClockStart;

//   		/* Just to make sure there were no problems */
//   		const bool sortTest = is_sorted(nums.begin(), nums.end());
// 	  	if (!sortTest) {
// 	  		cout << "There were problems with the bitonic sort!" << endl;
// 	  		MPI_Finalize();
// 	  		return EXIT_FAILURE;
// 	  	}
//   		cout << "Computation complete." << endl;
// 		cout << "Wall clock time used: " << diff << " seconds, which is the same as ";
// 		cout << diff / 60.f << " minutes." << endl;
//   	}

// 	MPI_Finalize();
// 	return EXIT_SUCCESS;
// }