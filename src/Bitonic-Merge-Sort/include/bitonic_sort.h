#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include <cmath>
#include <cstdlib> // for srand
#include <ctime> 
#include <iostream> 
#include <mpi.h>
#include <random>
#include <stdexcept>

#include <vector>

using std::vector;

template< typename T >
inline void generateListData(vector<T> & vec);

template< typename T >
inline void printList(const vector<T> & vec);

template< typename SortType, typename CompareType >
void bitonicSort(vector<SortType> & vec);

template< typename SortType, typename CompareType >
void quickSort(vector<SortType> & vec);

template < typename SortType, typename CompareType >
int partition(vector<SortType> & vec, int left, int right);

template < typename SortType, typename CompareType >
void qs(vector<SortType> & vec, const int left, const int right);

template <class SortType, class CompareType>
void sendMaxAndGetMin(vector<SortType> & vec, const int otherRank);

template <class SortType, class CompareType>
void sendMinAndGetMax(vector<SortType> & vec, const int otherRank);

template <typename SortType>
inline void customSwap(SortType & x, SortType & y);

#endif