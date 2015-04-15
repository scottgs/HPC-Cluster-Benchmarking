#ifndef SIMPLE_BITONIC_SORT_H
#define SIMPLE_BITONIC_SORT_H

#include <cmath>
#include <cstdlib> // for srand
#include <ctime> 
#include <iostream> 
#include <mpi.h>
#include <random>
#include <stdexcept>
#include <vector>

#include <boost/lexical_cast.hpp>

using std::vector;

template < typename T >
T path_and_check(int argc, char ** argv);

template< typename T >
inline void generate_list_data(vector<T> & vec);

template<typename T>
inline void print_list(const vector<T> & vec);

template < typename T >
void bitonic_sort(vector<T> & nums);

#endif