// #include "../include/bitonic_sort.h"

// #define MASTER_NODE 0

// /* Tags for send and receive */
// static const int MAX_MESSAGE_TAG = 3;
// static const int MIN_MESSAGE_TAG = 4;
// static const int MIN_TAG = 5;
// static const int MAX_TAG = 6;

// using namespace std;

// template< typename T >
// inline void generateListData(vector<T> & vec) {
// 	std::random_device rd;
//     std::mt19937 mt(rd());
//     std::uniform_real_distribution<double> dist(1.f, 100.f);

//     for (size_t i = 0; i < vec.size(); ++i) {
//     	vec[i] = dist(mt);
//     }
// }

// #ifdef NUMS_DEBUG
// template< typename T >
// inline void printList(const vector<T> & vec) {
// 	int rank;
// 	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
// 	cout << "Format is array[processRank][index] for printing purposes." << endl;
// 	for (size_t i = 0; i < vec.size(); ++i) {
// 		cout << "Vector[" << rank << "][" << i << "] = " << vec[i] << endl;
// 	}
// }
// #endif

// template < typename SortType >
// inline void customSwap(SortType & x, SortType & y) {
//   SortType temp = std::move(x);
//   x = std::move(y);
//   y = std::move(temp);
// }

// template < typename SortType, typename CompareType >
// int partition(vector<SortType> & vec, int left, int right) {
// 	if (left == right) return left;
// 	// TODO: Can improve way of picking pivot. Randomized partition is an option.
// 	SortType pivot = right;
// 	// Swap the pivot to the middle.
// 	customSwap(vec[pivot], vec[(left + right)/2]);
// 	--right;

// 	for(;;) {
// 		while(CompareType(vec[left]) < CompareType(vec[pivot])) 
// 			++left;
// 		while (right >= left && CompareType(vec[pivot]) <= CompareType(vec[right]))
// 			--right;
// 		if (right < left)
// 			break;

// 		customSwap(vec[left], vec[right]);
// 		++left;
// 		--right;
// 	}

// 	// Place the pivot to the right place
// 	customSwap(vec[pivot], vec[left]);
// 	return left;
// }

// template < class SortType >
// inline SortType getPivotIndex(const SortType left, const SortType right) {
// 	return left + (right - left + 1) / 2;
// }

// template < class SortType, class CompareType >
// void sendMaxReceiveMin(vector<SortType> & vec, const int otherRank) {
// 	const size_t size = vec.size();
// 	SortType left = -1,
// 			 right = size - 1,
// 			 pivot = left + (right - left + 1) / 2;
// 	CompareType tempCompare = CompareType(vec[pivot]),
// 				receiveCompare = -1; // just init to this--will get updated

// 	/* MPI_Sendrec can be thought as an MPI_Isend, MPI_Irecv, and a pair of MPI_Waits. */
// 	MPI_Sendrecv(
// 		&tempCompare,				// Init address of send buffer
// 		sizeof(CompareType),		// Number of elements in send buffer
// 		MPI_BYTE,					// Type of elements in send buffer
// 		otherRank,					// Destination process
// 		MIN_TAG,					// Send tag
// 		&receiveCompare,			// Init address of receive buffer
// 		sizeof(CompareType),		// Number of elements in receive buffer
// 		MPI_BYTE,					// Type of elements in receive buffer
// 		otherRank,					// Source process
// 		MAX_TAG,					// Receive tag
// 		MPI_COMM_WORLD,				// Communicator
// 		MPI_STATUS_IGNORE			// MPI Status
// 	);

// 	while (pivot != left && pivot != right && vec[pivot] != receiveCompare) {
// 		/* Compare the pivot value to the incoming one. */
// 		if (vec[pivot] < receiveCompare)
// 			left = pivot;
// 		else
// 			right = pivot;
// 		/* Update the pivot to send off. */
// 		pivot = left + (right - left + 1) / 2;
// 		try {
// 			MPI_Sendrecv(
// 				&tempCompare,				// Init address of send buffer
// 				sizeof(CompareType),		// Number of elements in send buffer
// 				MPI_BYTE,					// Type of elements in send buffer
// 				otherRank,					// Destination process
// 				MIN_TAG,					// Send tag
// 				&receiveCompare,			// Init address of receive buffer
// 				sizeof(CompareType),		// Number of elements in receive buffer
// 				MPI_BYTE,					// Type of elements in receive buffer
// 				otherRank,					// Source process
// 				MAX_TAG,					// Receive tag
// 				MPI_COMM_WORLD,				// Communicator
// 				MPI_STATUS_IGNORE			// MPI Status
// 			);
// 		}
// 		catch (MPI::Exception & e) {
// 			cout << "MPI Error: " << e.Get_error_string() << e.Get_error_code();
// 			MPI::COMM_WORLD.Abort(-1);
// 		}
// 	}

// 	if (receiveCompare <= vec[pivot]) {
// 		try {
// 			MPI_Sendrecv_replace(
// 				&vec[pivot], 						// Init address of send AND receive buffers
// 				(size - pivot) * sizeof(SortType),	// Number of elements in send AND receive buffers
// 				MPI_BYTE,							// Send datatype
//             	otherRank, 							// Destination process
//             	MIN_TAG,							// Send Tag
//             	otherRank,							// Source process
//             	MIN_MESSAGE_TAG,					// Receive Tag
//             	MPI_COMM_WORLD,						// Communicator
//             	MPI_STATUS_IGNORE					// Status
//             );
// 		}
// 		catch (MPI::Exception & e) {
// 			cout << "MPI Error: " << e.Get_error_string() << e.Get_error_code();
// 			MPI::COMM_WORLD.Abort(-1);
// 		}
// 	}

// 	else {
// 		if (pivot != size - 1) {
// 			try {
// 				MPI_Sendrecv_replace(
// 					&vec[pivot + 1], 						// Init address of send AND receive buffers
// 					(size - pivot -1) * sizeof(SortType),	// Number of elements in send AND receive buffers
// 					MPI_BYTE,								// Send datatype
// 	            	otherRank, 								// Destination process
// 	            	MIN_MESSAGE_TAG,						// Send Tag
// 	            	otherRank,								// Source process
// 	            	MAX_MESSAGE_TAG,						// Receive Tag
// 	            	MPI_COMM_WORLD,							// Communicator
// 	            	MPI_STATUS_IGNORE						// Status
// 	            );
// 			}
// 			catch (MPI::Exception & e) {
// 				cout << "MPI Error: " << e.Get_error_string() << e.Get_error_code();
// 				MPI::COMM_WORLD.Abort(-1);
// 			}
// 		}
// 	}
// }	

// template < class SortType, class CompareType >
// void sendMinReceiveMax(vector<SortType> & vec, const int otherRank) {
// 	const size_t size = vec.size();
// 	SortType left = 0,
// 			 right = size - 1,
// 			 pivot = getPivotIndex<SortType>(left, right);
// 	CompareType tempCompare = CompareType(vec[pivot]),
// 				receiveCompare = -1; // just init to this--will get updated

// 	/* MPI_Sendrec can be thought as an MPI_Isend, MPI_Irecv, and a pair of MPI_Waits. */
// 	MPI_Sendrecv(
// 		&tempCompare,				// Init address of send buffer
// 		sizeof(CompareType),		// Number of elements in send buffer
// 		MPI_BYTE,					// Type of elements in send buffer
// 		otherRank,					// Destination process
// 		MAX_TAG,					// Send tag
// 		&receiveCompare,			// Init address of receive buffer
// 		sizeof(CompareType),		// Number of elements in receive buffer
// 		MPI_BYTE,					// Type of elements in receive buffer
// 		otherRank,					// Source process
// 		MIN_TAG,					// Receive tag
// 		MPI_COMM_WORLD,				// Communicator
// 		MPI_STATUS_IGNORE			// MPI Status
// 	);

// 	while (pivot != left && vec[pivot] != receiveCompare) {
// 		if (vec[pivot] < receiveCompare)
// 			left = pivot;
// 		else
// 			right = pivot;
		
// 		pivot = getPivotIndex<SortType>(left, right);
// 		tempCompare = CompareType(vec[pivot]);

// 		MPI_Sendrecv(
// 			&tempCompare,				// Init address of send buffer
// 			sizeof(CompareType),		// Number of elements in send buffer
// 			MPI_BYTE,					// Type of elements in send buffer
// 			otherRank,					// Destination process
// 			MAX_TAG,					// Send tag
// 			&receiveCompare,			// Init address of receive buffer
// 			sizeof(CompareType),		// Number of elements in receive buffer
// 			MPI_BYTE,					// Type of elements in receive buffer
// 			otherRank,					// Source process
// 			MIN_TAG,					// Receive tag
// 			MPI_COMM_WORLD,				// Communicator
// 			MPI_STATUS_IGNORE			// MPI Status
// 		);
// 	}

// 	if (vec[pivot] <= receiveCompare) {
// 		MPI_Sendrecv_replace(
// 				&vec[0], 							// Init address of send AND receive buffers
// 				(pivot + 1) * sizeof(SortType),		// Number of elements in send AND receive buffers
// 				MPI_BYTE,							// Send datatype
//             	otherRank, 							// Destination process
//             	MAX_MESSAGE_TAG,					// Send Tag
//             	otherRank,							// Source process
//             	MIN_MESSAGE_TAG,					// Receive Tag
//             	MPI_COMM_WORLD,						// Communicator
//             	MPI_STATUS_IGNORE					// Status
// 		);
// 	} 
// 	else if (pivot != 0) {
// 		MPI_Sendrecv_replace(
// 			&vec[0], 						// Init address of send AND receive buffers
// 			pivot * sizeof(SortType),		// Number of elements in send AND receive buffers
// 			MPI_BYTE,						// Send datatype
//         	otherRank, 						// Destination process
//         	MAX_MESSAGE_TAG,				// Send Tag
//         	otherRank,						// Source process
//         	MIN_MESSAGE_TAG,				// Receive Tag
//         	MPI_COMM_WORLD,					// Communicator
//         	MPI_STATUS_IGNORE				// Status
//         );
// 	}
// }

// // Reference: http://web.mst.edu/~ercal/387/P3/pr-proj-3.pdf
// template< typename SortType, typename CompareType >
// inline void bitonicSort(vector<SortType> & vec, const int numProcs, const int rank) {
// 	quickSort<SortType, CompareType>(vec);
// 	const uint logNumProcs = log2(numProcs);

// 	for (size_t bitIndex = 1; bitIndex <= logNumProcs; ++bitIndex) {
// 		const size_t diBit = (rank >> bitIndex) & 0x1;
// 		for (size_t otherBit = bitIndex - 1; otherBit >= 0; --otherBit) {
// 			const size_t otherDiBit = (rank >> otherBit) & 0x1;
// 			const size_t otherRank = rank ^ (1 << otherBit);

// 			if (diBit != otherDiBit) {
// 				sendMinReceiveMax<SortType, CompareType>(vec, otherRank);
// 			}
// 			else {
// 				sendMaxReceiveMin<SortType, CompareType>(vec, otherRank);
// 			}

// 			quickSort<SortType, CompareType>(vec);
// 		}
// 	}
// }

// template < typename SortType, typename CompareType >
// void qs(vector<SortType> & vec, const int left, const int right) {
// 	if (left < right) {
// 		const int pivot = partition<SortType, CompareType>(vec, left, right);
// 		// Call qs on the left and right halves
// 		qs<SortType, CompareType>(vec, left, pivot - 1);
// 		qs<SortType, CompareType>(vec, pivot + 1, right);
// 	}
// }

// template< typename SortType, typename CompareType >
// void quickSort(vector<SortType> & vec) {
// 	qs<SortType, CompareType>(vec, 0, vec.size());
// }

// template < typename T >
// T path_and_check(int argc, char ** argv) {
// 	if (argc < 2) {
// 		cerr << "Not enough parameters!" << endl;
// 		cerr << "To run the program, use mpirun -np NUM_PROCESSORS path/to/binary NUM_ITEMS_TO_SORT." << endl; 
// 		return -1;
// 	}
	
// 	T listSize;
// 	try {
// 		listSize = boost::lexical_cast<unsigned long>(argv[1]);
// 	}
// 	catch(boost::bad_lexical_cast & e) {
// 		throw runtime_error("Number of elements to sort must be an integer > 0.");
// 		return EXIT_FAILURE;
// 	}	
// 	return listSize;
// }

// int main(int argc, char ** argv) {
// 	// Initialize the MPI Environment and usual jazz
// 	MPI_Init(&argc, &argv);

// 	// Rank for each process and the total number of processes
// 	int procRank, numProcs;
// 	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
// 	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

// 	const unsigned long listSize = path_and_check<unsigned long>(argc, argv);

// 	vector<int> nums(listSize);

// 	// Generate some random data to play with
// 	srand(static_cast<unsigned int>(time(NULL)));
// 	generateListData<int>(nums);
// 	#ifdef NUMS_DEBUG
// 		printList(nums);
// 	#endif
	
// 	bitonicSort<int, int>(nums, numProcs, procRank);
// 	// #ifdef NUMS_DEBUG
// 	// 	printList(nums);
// 	// #endif	

// 	bool isSorted = is_sorted(nums.begin(), nums.end());
// 	bool locallySorted = false;

// 	/* Perform a logical AND across the elements. */
// 	MPI_Reduce(
// 		&isSorted,			// Send buffer
// 		&locallySorted,		// Receive buffer
// 		1,					// Number of elements in send buffer
// 		MPI_INT,			// MPI_Datatype for send buffer, i.e. what is in our list to srt
// 		MPI_LAND,			// MPI_Op, i.e. reduce operation. Logical AND operator here.
// 		MASTER_NODE,		// Root process
// 		MPI_COMM_WORLD 		// Communicator
// 	);

// 	int * const extremum = new int[2 * numProcs];
// 	int extrem[2];
// 	extrem[0] = nums[0];
// 	extrem[1] = nums[listSize-1];

// 	/*
// 	 * Each process (root process included) sends the contents of its send buffer to the root process. 
// 	 * The root process receives the messages and stores them in rank order. 
// 	 * The outcome is as if each of the n processes in the group (including the root process) had executed a call to
// 	 * MPI_Send and MPI_Recv.
// 	 */
// 	MPI_Gather(
// 		extrem,				// Starting address of send buffer
// 		2,					// Number of elements in send buffer
// 		MPI_INT,			// Datatype of send buffer
// 		extremum,			// Address of receive buffer
// 		2,					// Number of elements for any single receive
// 		MPI_INT,			// Datatype of receive buffer elements
// 		MASTER_NODE,		// Rank of receiving process
// 		MPI_COMM_WORLD 		// Communicator
// 	);
	
// 	#ifdef _DEBUG
// 		if (isSorted)
// 			cout << "Is sorted!\n" << endl;
// 		else
// 			cout << "NO, it is not sorted!\n" << endl;
// 	#endif

// 	if (procRank == MASTER_NODE) {
// 		if (locallySorted)
// 			cout << "Locally sorted!\n" << endl;
// 		else 
// 			cout << "NO, not locally sorted!\n" << endl;
	
// 		bool extremumCheck = false;
// 		for (int procIndex = 1; procIndex < numProcs && extremumCheck; ++procIndex) {
// 			if (extremum[2 * (procIndex - 1) + 1] > extremum[2 * procIndex])
// 				extremumCheck = false;
// 		}

// 		if (extremumCheck)
// 			cout << "No problems with the extremum.\n" << endl;
// 		else
// 			cout << "Problem with the extremum.\n" << endl;
// 	}

// 	// Free up my resources used
// 	delete[] extremum;

// 	// Shutdown MPI and release those resources
// 	MPI_Finalize();

// 	return 0;
// }