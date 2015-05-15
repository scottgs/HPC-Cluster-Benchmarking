#include "../include/bitonic_sort.h"

#define MASTER_NODE 0

/* Tags for send and receive for MPI Flags.*/
static const int MAX_MESSAGE_TAG = 3;
static const int MIN_MESSAGE_TAG = 4;
static const int MIN_TAG = 5;
static const int MAX_TAG = 6;

using namespace std;

template< typename T >
inline void generateListData(vector<T> & vec) {
	std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(1.f, 100.f);

    for (size_t i = 0; i < vec.size(); ++i) {
    	vec[i] = dist(mt);
    }
}

#ifdef NUMS_DEBUG
template< typename T >
inline void printList(const vector<T> & vec) {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	cout << "Format is vec[processRank][index] for printing purposes." << endl;
	for (size_t i = 0; i < vec.size(); ++i) {
		cout << "Vector[" << rank << "][" << i << "] = " << vec[i] << endl;
	}
}
#endif

template < typename SortType >
inline void customSwap(SortType & x, SortType & y) {
  SortType temp = std::move(x);
  x = std::move(y);
  y = std::move(temp);
}

template <typename SortType, typename CompareType>
int partition(vector<SortType> & vec, int left, int right){
   if (left == right) return left;
	// TODO: Can improve way of picking pivot. Randomized partition is an option.
	SortType pivot = right;
	// Swap the pivot to the middle.
	customSwap(vec[pivot], vec[(left + right)/2]);
	--right;

	for(;;) {
		while(CompareType(vec[left]) < CompareType(vec[pivot])) 
			++left;
		while (right >= left && CompareType(vec[pivot]) <= CompareType(vec[right]))
			--right;
		if (right < left)
			break;

		customSwap(vec[left], vec[right]);
		++left;
		--right;
	}

	// Place the pivot to the right place
	customSwap(vec[pivot], vec[left]);
	return left;
}
 
template <typename SortType, typename CompareType>
void qs(vector<SortType> & vec, const int left, const int right){
    if(left < right){
        const SortType pivot = partition<SortType,CompareType>(vec, left, right);
        qs<SortType,CompareType>(vec, pivot + 1, right);
        qs<SortType,CompareType>(vec, left, pivot - 1);
    }
}
 
template <typename SortType, typename CompareType>
void quicksort(vector<SortType> & vec){
    qs<SortType,CompareType>(vec, 0, vec.size() - 1);
}

template < typename SortType >
inline SortType getPivotIndex(const SortType left, const SortType right) {
	return left + (right - left + 1) / 2;
}

/**
 * Exchanges data with the other rank.
 * Sends the max value and receives the min value.
 */
template <typename SortType, typename CompareType>
void sendMaxReceiveMin (vector<SortType> & vec, const int otherRank) {
	const SortType size = vec.size();
    SortType left  = -1;
    SortType right = size - 1;
    SortType pivot = getPivotIndex<SortType>(left, right);

    CompareType otherValue = -1; // just init to this--will get updated. This is the other rank's value used for comparison
    CompareType tempCompareValue = CompareType(vec[pivot]); 

    /* MPI_Sendrec can be thought as an MPI_Isend, MPI_Irecv, and a pair of MPI_Waits. */
    MPI_Sendrecv(
    	&tempCompareValue,
    	sizeof(CompareType),
    	MPI_BYTE,
    	otherRank,
    	MIN_TAG,
    	&otherValue,
    	sizeof(CompareType),
    	MPI_BYTE,
    	otherRank,
    	MAX_MESSAGE_TAG,
    	MPI_COMM_WORLD,
    	MPI_STATUS_IGNORE
    );
 
    while(pivot != left && pivot != right  && vec[pivot] != otherValue) {
    	/* Compare the pivot value to the incoming one. */
    	if (vec[pivot] < otherValue)
    		left = pivot;
    	else 
    		right = pivot;


		/* Update the pivot to send off. */
        pivot = getPivotIndex<SortType>(left, right);
        tempCompareValue = CompareType(vec[pivot]);
 
        MPI_Sendrecv(
        	&tempCompareValue,
        	sizeof(CompareType),
        	MPI_BYTE,
        	otherRank,
        	MIN_TAG,
        	&otherValue,
        	sizeof(CompareType),
        	MPI_BYTE,
        	otherRank,
        	MAX_MESSAGE_TAG,
        	MPI_COMM_WORLD,
        	MPI_STATUS_IGNORE
        );
    }
 
    if(otherValue <= vec[pivot]) {
        MPI_Sendrecv_replace(
        	&vec[pivot],
        	(size - pivot) * sizeof(SortType),
        	MPI_BYTE,
        	otherRank,
        	MIN_MESSAGE_TAG,
        	otherRank,
        	MAX_MESSAGE_TAG,
        	MPI_COMM_WORLD,
        	MPI_STATUS_IGNORE
        ); 
    }
    else if(vec[pivot] < otherValue) {
        if(pivot != size - 1){
            MPI_Sendrecv_replace(
            &vec[pivot + 1], 
            (size - pivot - 1) * sizeof(SortType), 
            MPI_BYTE,
            otherRank, 
            MIN_MESSAGE_TAG, 
            otherRank, 
            MAX_MESSAGE_TAG,
            MPI_COMM_WORLD, 
            MPI_STATUS_IGNORE);
        }
    }
 
}

/**
 * Exchanges data with the other rank.
 * Sends the min value and receives the max value.
 */
template <typename SortType, typename CompareType>
void sendMinReceiveMax(vector<SortType> & vec, const int otherRank) {
    SortType left  = 0;
    const SortType size = vec.size();
    SortType right = size;
    SortType pivot = getPivotIndex<SortType>(left, right);
    CompareType otherValue = -1;
    CompareType tempCompareValue = CompareType(vec[pivot]);

    MPI_Sendrecv(
    	&tempCompareValue,
    	sizeof(CompareType),
    	MPI_BYTE,
    	otherRank,
    	MAX_MESSAGE_TAG,
    	&otherValue,
    	sizeof(CompareType),
    	MPI_BYTE,
    	otherRank,
    	MIN_TAG,
    	MPI_COMM_WORLD,
    	MPI_STATUS_IGNORE
    );
 
    while(pivot != left  && vec[pivot] != otherValue) {
        if(vec[pivot] < otherValue)
            left = pivot;
        else
            right = pivot;
    
        pivot = left + (right - left)/2;

        tempCompareValue = CompareType(vec[pivot]);

        MPI_Sendrecv(
    		&tempCompareValue,
    		sizeof(CompareType),
    		MPI_BYTE,
    		otherRank,
    		MAX_MESSAGE_TAG,
    		&otherValue,
    		sizeof(CompareType),
    		MPI_BYTE,
    		otherRank,
    		MIN_TAG,
    		MPI_COMM_WORLD,
    		MPI_STATUS_IGNORE
    	);
    }
 
 
    if(vec[pivot] <= otherValue){
         MPI_Sendrecv_replace(
        	&vec[0],
        	(pivot + 1) * sizeof(SortType),
        	MPI_BYTE,
            otherRank,
            MAX_MESSAGE_TAG,
            otherRank,
            MIN_MESSAGE_TAG,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    else if(otherValue < vec[pivot]) {
        if(pivot != 0) {
            MPI_Sendrecv_replace(
	            &vec[0],
	            (pivot) * sizeof(SortType),
	            MPI_BYTE,
	            otherRank,
	            MAX_MESSAGE_TAG,
	            otherRank,
	            MIN_MESSAGE_TAG,
	            MPI_COMM_WORLD,
	            MPI_STATUS_IGNORE
	        );
        }
    }
}

// Reference: http://web.mst.edu/~ercal/387/P3/pr-proj-3.pdf
template <typename SortType, typename CompareType>
void bitonic(vector<SortType> & vec, const int num_procs, const int rank){
    quicksort<SortType,CompareType>(vec);
 
    const int num_steps = log2(num_procs);
    for(int bit_index = 1 ; bit_index <= num_steps ; ++bit_index){
        // Window ID is the most signiciant (d-i) bits
		const int diBit = (rank >> bit_index) & 0x1;
		for (int j = bit_index - 1; j >= 0; --j) {
			// if window id is even AND jth bit of kth processor = 0
			// OR window is is odd AND jth bit of kth processor = 1

			const int otherDiBit = (rank >> j) & 0x1;
			const int swapPartner = rank ^ (1 << j);

			if (diBit != otherDiBit) {
				sendMinReceiveMax<SortType, CompareType>(vec, swapPartner);
			}
			else {
				sendMaxReceiveMin<SortType, CompareType>(vec, swapPartner);
			}

			quicksort<SortType, CompareType>(vec);
		}
    }
}

template < typename T >
T path_and_check(int argc, char ** argv) {
	if (argc < 2) {
		cerr << "Not enough parameters!" << endl;
		cerr << "To run the program, use mpirun -np NUM_PROCESSORS path/to/binary NUM_ITEMS_TO_SORT." << endl; 
		return -1;
	}
	
	T listSize;
	try {
		listSize = boost::lexical_cast<unsigned long>(argv[1]);
	}
	catch(boost::bad_lexical_cast & e) {
		throw runtime_error("Number of elements to sort must be an integer > 0.");
		return EXIT_FAILURE;
	}	
	return listSize;
}

int main(int argc, char ** argv) {
	// Initialize the MPI Environment and usual jazz
	MPI_Init(&argc, &argv);

	// Rank for each process and the total number of processes
	int procRank, numProcs;
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

	const unsigned long listSize = path_and_check<unsigned long>(argc, argv);

	vector<int> nums(listSize);

	// Generate some random data to play with
	srand(static_cast<unsigned int>(time(NULL)));
	generateListData<int>(nums);
	#ifdef NUMS_DEBUG
		printList(nums);
	#endif
	
	bitonic<int, int>(nums, numProcs, procRank);

    bool isSorted = is_sorted(nums.begin(), nums.end());
    bool locallySorted = false;

    /* Perform a logical AND across the elements. */
    MPI_Reduce(&isSorted, &locallySorted, 1, MPI_INT, MPI_LAND , 0, MPI_COMM_WORLD);
 
    int * const allExtremum = new int[numProcs * 2];
    int extremum[2];
    extremum[0] = nums[0];
    extremum[1] = nums[listSize-1];

    /*
	 * Each process (root process included) sends the contents of its send buffer to the root process. 
	 * The root process receives the messages and stores them in rank order. 
	 * The outcome is as if each of the n processes in the group (including the root process) had executed a call to
	 * MPI_Send and MPI_Recv.
	 */
    MPI_Gather(extremum, 2, MPI_INT, allExtremum, 2, MPI_INT, 0, MPI_COMM_WORLD);
 
    #ifdef _DEBUG
		if (isSorted)
			cout << "Is sorted!" << endl;
		else
			cout << "NO, it is not sorted!" << endl;
	#endif
 
    if(procRank == MASTER_NODE){
    	#ifdef BITONC_DEBUG
	       	bool extremumCheck = true;
			for (int procIndex = 1; procIndex < numProcs && extremumCheck; ++procIndex) {
				if (extremum[2 * (procIndex - 1) + 1] > extremum[2 * procIndex])
					extremumCheck = false;
			}

			if (extremumCheck)
				cout << "No problems with the extremum." << endl;
			else
				cout << "Problem with the extremum.\n" << endl;
 		#endif
    }
 	
 	// Free up my resources used
    delete[] allExtremum;
 
	// Shutdown MPI and release those resources
	MPI_Finalize();

	return EXIT_SUCCESS;
}