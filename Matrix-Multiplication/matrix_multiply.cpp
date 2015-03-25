#include <assert.h>
#include <chrono>
#include <iostream>
#include <limits> // for INT_MAX

#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>

#include "matrix.h"

inline int gen_random_int() {
	boost::mt19937 rng;
    boost::uniform_real<int> u(0, INT_MAX);
    boost::variate_generator<boost::mt19937&, boost::uniform_real<int> > int_gen(rng, u);
    return int_gen();
}

inline void generate_random_vector(Matrix & matrix) {
    const uint num_rows = matrix.size.rows;
	const uint num_cols = matrix.size.cols;

	for (uint i = 0; i < num_rows; ++i) {
		for (uint j = 0; j < num_cols; ++j) {
			matrix.data[i][j] = gen_random_int(); 
		}
	}
}

// TODO: use processor timings rather than waiting times
int main(int argc, const char* argv[]) {
	if (argc < 2) {
		std::cout << "Invalid number of arguments. The second argument should be the size of the matrices to multiply." << std::endl;
		exit(1);
	}

	const uint size = boost::lexical_cast<uint>(argv[1]);

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