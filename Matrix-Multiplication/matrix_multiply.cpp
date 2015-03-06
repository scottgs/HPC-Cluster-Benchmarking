#include <assert.h>
#include <chrono>
#include <iostream>

#include "matrix.h"

inline void generate_random_vector(Matrix & matrix) {
	// use a constant number to seed the pseudo random number generator
	// this way your results at least have the same input on a given system
	srand(1337);
	const uint num_rows = matrix.size.rows;
	const uint num_cols = matrix.size.cols;

	for (uint i = 0; i < num_rows; ++i) {
		for (uint j = 0; j < num_cols; ++j) {
			matrix.data[i][j] = rand(); 
		}
	}
}

// TODO: use processor timings rather than waiting times
int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Invalid number of arguments. The second argument should be the size of the matrices to multiply." << std::endl;
		exit(1);
	}

	const uint size = std::stoi(std::string(argv[1]));

	Matrix A(Size(size, size));
	Matrix B(Size(size, size));

	// Generate some random test data
	generate_random_vector(A);
	generate_random_vector(B);

	// Time the multiplication for sequential
	std::chrono::time_point<std::chrono::system_clock> start_time, end_time;
	start_time = std::chrono::system_clock::now();

	Matrix C = A*B;

	end_time = std::chrono::system_clock::now();
	double diff = std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1>>>(end_time - start_time).count();

	std::cout << "It took " << diff << " seconds to do the multiplication." << std::endl;

	return 0;
}