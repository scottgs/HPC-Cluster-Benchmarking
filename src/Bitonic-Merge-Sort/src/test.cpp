// #include <cstdio>
// #include <cstdlib>
// #include <cmath>
 
// #include <mpi.h>
 
 
// ////////////////////////////////////////////////////////////////
// // Sequential sort first!
// ////////////////////////////////////////////////////////////////
 
// template <class SortType>
// inline void swap(SortType& v1, SortType& v2){
//     const SortType tmp = v1;
//     v1 = v2;
//     v2 = tmp;
// }
 
// template <class SortType, class CompareType, class FSize>
// int partition(SortType* const array, FSize left, FSize right){
//     SortType part = right;
//     swap(array[part],array[(right+left) / 2]);
//     --right;
 
//     while(true){
//         while( CompareType(array[left]) < CompareType(array[part])){
//             ++left;
//         }
//         while(right >= left && CompareType(array[part]) <= CompareType(array[right])){
//             --right;
//         }
//         if(right < left) break;
 
//         swap(array[left],array[right]);
//         ++left;
//         --right;
//     }
 
//     swap(array[part],array[left]);
 
//     return left;
// }
 
// template <class SortType, class CompareType, class FSize>
// void qs(SortType* const array, const FSize left, const FSize right){
//     if(left < right){
//         const FSize part = partition<SortType,CompareType>(array, left, right);
//         qs<SortType,CompareType>(array,part + 1,right);
//         qs<SortType,CompareType>(array,left,part - 1);
//     }
// }
 
// template <class SortType, class CompareType, class FSize>
// void quick(SortType* const array, const FSize size){
//     qs<SortType,CompareType>(array,0,size-1);
// }
 
// ////////////////////////////////////////////////////////////////
// // Bitonic parallel sort !
// ////////////////////////////////////////////////////////////////
 
// // Mpi flag
// static const int FlagMin = 5;
// static const int FlagMax = 6;
// static const int FlagMinMess = 4;
// static const int FlagMaxMess = 3;
 
// // This function exchange data with the other rank,
// // its send the max value and receive min value
// template <class SortType, class CompareType, class FSize>
// void sendMaxAndGetMin(SortType array[], const FSize size, const int otherRank){
//     FSize left  = -1;
//     FSize right = size - 1;
//     FSize pivot = left + (right - left + 1)/2;
//     CompareType otherValue = -1;
//     CompareType tempCompareValue = CompareType(array[pivot]);
//     MPI_Sendrecv(&tempCompareValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMin,&otherValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMax,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
 
//     while( pivot != left && pivot != right  && array[pivot] != otherValue) {
 
//         if( array[pivot] < otherValue ){
//             left = pivot;
//         }
//         else {
//             right = pivot;
//         }
//         pivot = left + (right - left + 1)/2;
//         tempCompareValue = CompareType(array[pivot]);
 
//         MPI_Sendrecv(&tempCompareValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMin,&otherValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMax,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//     }
 
//     if( otherValue <= array[pivot] ){
//         MPI_Sendrecv_replace(&array[pivot], (size - pivot) * sizeof(SortType) , MPI_BYTE,
//                                otherRank, FlagMinMess, otherRank, FlagMaxMess,
//                                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
 
//     }
//     else if( array[pivot] < otherValue){
//         if(pivot != size - 1){
//             MPI_Sendrecv_replace(&array[pivot + 1], (size - pivot - 1) * sizeof(SortType) , MPI_BYTE,
//                                    otherRank, FlagMinMess, otherRank, FlagMaxMess,
//                                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//         }
//     }
 
// }
 
// // This function exchange data with the other rank,
// // its send the min value and receive max value
// template <class SortType, class CompareType, class FSize>
// void sendMinAndGetMax(SortType array[], const FSize size, const int otherRank){
//     FSize left  = 0;
//     FSize right = size ;
//     FSize pivot = left + (right - left)/2;
//     CompareType otherValue = -1;
//     CompareType tempCompareValue = CompareType(array[pivot]);
//     MPI_Sendrecv(&tempCompareValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMax,&otherValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMin,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
 
//     while(  pivot != left  && array[pivot] != otherValue) {
 
//         if( array[pivot] < otherValue ){
//             left = pivot;
//         }
//         else {
//             right = pivot;
//         }
//         pivot = left + (right - left)/2;
//         tempCompareValue = CompareType(array[pivot]);
//         MPI_Sendrecv(&tempCompareValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMax,&otherValue,sizeof(CompareType),MPI_BYTE,otherRank,FlagMin,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
//     }
 
 
//     if( array[pivot] <= otherValue ){
//         MPI_Sendrecv_replace(&array[0], (pivot + 1) * sizeof(SortType) , MPI_BYTE,
//                                otherRank, FlagMaxMess, otherRank, FlagMinMess,
//                                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//     }
//     else if( otherValue < array[pivot]){
//         if(pivot != 0){
//             MPI_Sendrecv_replace(&array[0], (pivot) * sizeof(SortType) , MPI_BYTE,
//                                    otherRank, FlagMaxMess, otherRank, FlagMinMess,
//                                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//         }
//     }
// }
 
// /*
// From :
 
// http://web.mst.edu/~ercal/387/P3/pr-proj-3.pdf
 
// Parallel Bitonic Sort Algorithm for processor Pk (for k := 0 . . . P − 1)
// d:= log P
// // cube dimension
// sort(local − datak ) // sequential sort
// // Bitonic Sort follows
// for i:=1 to d do
//     window-id = Most Significant (d-i) bits of Pk
//     for j:=(i-1) down to 0 do
//         if((window-id is even AND j th bit of Pk = 0)
//         OR (window-id is odd AND j th bit of Pk = 1))
//             then call CompareLow(j)
//         else
//             call CompareHigh(j)
//         endif
//     endfor
// endfor
//   */
// template <class SortType, class CompareType, class FSize>
// void bitonic(SortType array[], const FSize size, const int np, const int rank){
//     quick<SortType,CompareType>(array, size);
 
//     const int logNp = log2(np);
//     for(int bitIdx = 1 ; bitIdx <= logNp ; ++bitIdx){
//         // window-id = Most Significant (d-i) bits of Pk
//         const int diBit =  (rank >> bitIdx) & 0x1;
 
//         for(int otherBit = bitIdx - 1 ; otherBit >= 0 ; --otherBit){
//             // if((window-id is even AND j th bit of Pk = 0)
//             // OR (window-id is odd AND j th bit of Pk = 1))
 
//             const int myOtherBit = (rank >> otherBit) & 0x1;
//             const int otherRank = rank ^ (1 << otherBit);
 
//             if( diBit != myOtherBit ){
//                 sendMinAndGetMax<SortType,CompareType>(array, size, otherRank);
//             }
//             else{
//                 sendMaxAndGetMin<SortType,CompareType>(array, size, otherRank);
//             }
//             // A merge sort is possible since the array is composed
//             // by two part already sorted, but we want to do this in space
//             quick<SortType,CompareType>(array, size);
//         }
//     }
// }
 
// ////////////////////////////////////////////////////////////////
// // Utils
// ////////////////////////////////////////////////////////////////
 
// template <class SortType, class FSize>
// bool isSorted(const SortType array[], const FSize size){
//     for(int idx = 1 ; idx < size ; ++idx){
//         if( array[idx-1] > array[idx]){
//             return false;
//         }
//     }
//     return true;
// }
 
// void print(const int array[], const int size, const int rank){
//     for(int idx = 0 ; idx < size ; ++idx){
//         printf("array[%d][%d] = %dn", rank, idx, array[idx]);
//     }
// }
 
 
// int main(int argc, char ** argv){
//     MPI_Init(&argc, &argv);
 
//     int rank = 0;
//     int nprocs = 0;
 
//     MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
//     MPI_Comm_rank(MPI_COMM_WORLD,&rank);
 
//     const int Size = 500;
//     long long array[Size];
//     srand(Size);
 
//     for(int idx = 0 ; idx < Size ; ++idx){
//         //array[idx] = nprocs - rank;
//         //array[idx] = rank;
//         array[idx] = Size * (rand()/float(RAND_MAX));
//     }
 
//     bitonic<long long,int>(array, Size, nprocs, rank);
//     //print(array, Size, rank);
 
//     int sorted = isSorted(array,Size);
//     bool localySorted = false;
//     MPI_Reduce( &sorted, &localySorted, 1, MPI_INT, MPI_LAND , 0, MPI_COMM_WORLD );
 
//     int*const allExtrem = new int[nprocs * 2];
//     int extrem[2];
//     extrem[0] = array[0];
//     extrem[1] = array[Size-1];
//     MPI_Gather(extrem, 2, MPI_INT, allExtrem, 2, MPI_INT, 0, MPI_COMM_WORLD);
 
//     printf(sorted?"Is sortedn":"NO is not sorted\n");
 
//     if( rank == 0){
//         printf(localySorted?"All sortedn":"NO all not sorted\n");
 
//         int extremOk = true;
//         for(int idxProc = 1 ; idxProc < nprocs && extremOk; ++idxProc){
//             if( allExtrem[2 * (idxProc - 1) + 1] > allExtrem[2 * idxProc]){
//                 extremOk = false;
//             }
//         }
 
//         printf(extremOk?"Extrem okn":"NO extrem error\n");
//     }
 
//     delete[] allExtrem;
 
//     MPI_Finalize();
 
//     return 0;
// }