#include <boost/lexical_cast.hpp>

#include "../include/bitonic_sort.h"

#define _DEBUG

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

#ifdef _DEBUG
template< typename T >
inline void printList(const vector<T> & vec) {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	cout << "Format is array[processRank][index] for printing purposes." << endl;
	for (size_t i = 0; i < vec.size(); ++i) {
		cout << "Vector[" << rank << "][" << i << "] = " << vec[i] << endl;
	}
}
#endif

template <typename SortType>
inline void customSwap(SortType & x, SortType & y) {
  const SortType temp = std::move(x);
  x = std::move(y);
  y = std::move(temp);
}

// TODO: make size types templated
template < typename SortType, typename CompareType >
int partition(vector<SortType> & vec, int left, int right) {
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

template <class SortType, class CompareType>
void sendMaxAndGetMin(vector<SortType> & vec, const int otherRank) {

}

template <class SortType, class CompareType>
void sendMinAndGetMax(vector<SortType> & vec, const int otherRank) {

}

// TODO: finish
// Reference: http://web.mst.edu/~ercal/387/P3/pr-proj-3.pdf
template< typename SortType, typename CompareType >
inline void bitonicSort(vector<SortType> & vec, const int numProcs, const int rank) {
	quickSort<SortType, CompareType>(vec);
	const uint logNumProcs = log2(numProcs);

	for (size_t bitIndex = 1; bitIndex <= logNumProcs; ++bitIndex) {
		const size_t diBit = (rank >> bitIndex) & 0x1;
		for (size_t otherBit = bitIndex -1; otherBit >= 0; --otherBit) {
			const size_t otherDiBit = (rank >> otherBit) & 0x1;
			const size_t otherRank = rank ^ (1 << otherBit);

			if (diBit != otherDiBit) {
				sendMinAndGetMax<SortType, CompareType>(vec, otherRank);
			}
			else {
				sendMaxAndGetMin<SortType, CompareType>(vec, otherRank);
			}

			quickSort<SortType, CompareType>(vec);
		}
	}
}

// TODO: make size types templated
template < typename SortType, typename CompareType >
void qs(vector<SortType> & vec, const int left, const int right) {
	if (left < right) {
		const int pivot = partition<SortType, CompareType>(vec, left, right);
		// Call qs on the left and right halves
		qs<SortType, CompareType>(vec, left, pivot - 1);
		qs<SortType, CompareType>(vec, pivot + 1, right);
	}
}

template< typename SortType, typename CompareType >
void quickSort(vector<SortType> & vec) {
	qs<SortType, CompareType>(vec, 0, vec.size());
}

int main(int argc, char ** argv) {
	// Initialize the MPI Environment and usual jazz
	MPI_Init(&argc, &argv);

	// Rank for each process and the total number of processes
	int procRank, numProcs;
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &procRank);

	if (argc < 2) {
		cerr << "Not enough parameters!" << endl;
		cerr << "To run the program, use mpirun -np NUM_PROCESSORS path/to/binary NUM_ITEMS_TO_SORT." << endl; 
		return -1;
	}
	const unsigned long listSize = boost::lexical_cast<unsigned long>(argv[1]);	
	if (listSize < 0) {
		throw runtime_error("Number of elements to sort must be > 0.");
		MPI_Finalize();
	}
	std::vector<double> nums(listSize);

	// Generate some random data to play with
	srand(static_cast<unsigned int>(time(NULL)));
	generateListData<double>(nums);
#ifdef _DEBUG
	printList(nums);
#endif
	
	bitonicSort<double, int>(nums, numProcs, procRank);
		

	MPI_Finalize();

	return 0;
}