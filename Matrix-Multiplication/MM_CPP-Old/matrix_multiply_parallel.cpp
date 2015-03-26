#include <boost/lexical_cast.hpp>
#include <boost/mpi.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/numeric/ublas/matrix.hpp> // to test against
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/serialization/serialization.hpp>

#include <chrono>
#include <iostream>
#include <limits> // for INT_MAX

#include "include/matrix.h"

// #define _DEBUG

namespace mpi = boost::mpi;
namespace ublas = boost::numeric::ublas;

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

Matrix_Section get_section(const Matrix & mat, const uint row_begin_index, const uint col_begin_index, const uint N) {
	Matrix_Section section;
	section.row_index = row_begin_index;
	section.col_index = col_begin_index;

	for (uint i = 0; i < N; ++i) {
		section.row_data.push_back(mat.get_row(row_begin_index + i));
		section.col_data.push_back(mat.get_col(col_begin_index + i));
	}

	return section;
}

/*
	Brief: kicks off the slave nodes and does all of the computations for the Matrix Multiplcation
 	Return the total amount of time it took to do all of the work.
*/
double run_master(mpi::communicator world, const uint size, const uint grid_dimension, Matrix & result) {
	std::chrono::time_point<std::chrono::system_clock> start_time, end_time;
	start_time = std::chrono::system_clock::now();

	// Send
	Matrix A(Size(size, size));
	generate_random_vector(A);

	// Do sequential -- just square the matrix.
	if (0 == grid_dimension) {
		result = A * A; 
	}
	// Otherwise, parallelize.
	else {
		// Split the matrix up and send some work to slaves
		uint slave_id = 1;
		const uint sub_matrix_size = size / grid_dimension;

		for (uint i = 0; i < size; i += sub_matrix_size) {
			for (uint j = 0; j < size; j += sub_matrix_size) {
				Matrix_Section section = get_section(A, i, j, sub_matrix_size);
				world.send(slave_id, 0, section);
				++slave_id;
			}
		}
	}

	// Receive the results
	const uint num_slaves = world.size() - 1;
	for (uint i = 1; i <= num_slaves; ++i) {
		Matrix mat;
		world.recv(i, 0, mat);
		result.insert_sub_mat(mat);
	}

	// Finished up
	end_time = std::chrono::system_clock::now();
	double diff = std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1>>>(end_time - start_time).count();
	return diff;
}

void run_slave(mpi::communicator world) {
	// Receive the Matrix section
	Matrix_Section section;
	world.recv(0, 0, section);

	Matrix sub_matrix(Size(section.row_data.size(), section.row_data.size()));

	// Calculate the partial results by doing the matrix multiplication on the sub matrix.
	section.calculate_vector_prod(sub_matrix);

	world.send(0, 0, sub_matrix);
}

bool test_boost_matrix(const Matrix & result, const uint size) {
	// Square matrix to test against
  	ublas::matrix<float> test_matrix(size, size);
  	ublas::matrix<float> result_test_matrix(size, size);
  	result_test_matrix = ublas::prod(test_matrix, test_matrix);

  	const uint num_rows = result.size.rows;
  	const uint num_cols = result.size.cols;

  	// Run through each element and make sure they're the same.
  	for (uint i = 0; i < num_rows; ++i) {
  		for (uint j = 0; num_cols; ++j) {
  			if (result.data[i][j] != result_test_matrix(i,j)) 
  				return false;
  		}
  	}

  	return true;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "Invalid number of arguments. The second argument should be the size of the matrices to multiply." << std::endl;
		exit(1);
	}

	const uint size = boost::lexical_cast<unsigned int>(argv[1]);

	mpi::environment env(argc, argv);
  	mpi::communicator world;

  	uint grid_dimension = sqrt(world.size() - 1);
  	if (grid_dimension != 0)
  		assert(size % grid_dimension == 0);

  	double total_time = 0;
  	const uint rank = world.rank();
  	Matrix result(Size(size, size));
  	// Master
  	if (0 == rank) {
  		total_time = run_master(world, size, grid_dimension, result);
  	}
  	else {
  		run_slave(world);
  	}

  	#ifdef _DEBUG
	  	bool test_pass = test_boost_matrix(result, size);
	  	if (!test_pass) {
	  		std::cout << "Something went wrong in the matrix multiplication." << std::endl;
	  		return -1;
	  	}
	#endif																																																																																																																																																																																																																																																																																																																																																																							

  	std::cout << "The total amount of time spent for doing MPI Parallel Multiplication is: " << total_time << " seconds." << std::endl;
	return 0;
}