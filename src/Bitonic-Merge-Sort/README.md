### Discussion about Sorting in General ###
* Comparison-based sorting uses repeated compare/exchanges of elements and the lower bound for a sequential algorithm in time is O(N log N).
* Noncomparison-based sorting uses certain known properties of elements (such as binary representation or distribution) and the lower bound for time complexity is O(N).

### Bitonic Merge Sort ###
A Bitonic List is defined as a list with no more than one *local maximum* and no more than one *local minimum* -- note that endpoints must also be considered (i.e. wraparound).

Binary-Split: divides the list equally into two lists by comparing/exchanging each item on the first half with the corresponding item in the second half of the list.

Example: 					&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 24  10  15  9  4  2  5  8 	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 10  11  12  13  22  30  32  45

Result after Binary-Split:  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 10  11  12  9  4  2  5  8 	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 24  20  15  13  22  30  32  45

Note that each element in the first half is smaller than each element in the second half and that also each half is a bitonic list of length n/2.

Thus, if you keep applying the Binary-Split to each half repeatedly, you will get a sorted list.  This takes (log N) parallel steps to sort.

The algorithm can be extended to an **arbitrary list** rather than a **Bitonic List** by simply first obtaining a bitonic sequence and then using this algorithm.

To sort a bitonic list when N is not a power of 2: you can use as many processors as N' where N' is defined to be a power of 2 *and* the smaller number larger than N. Then, assign a special value to all the extra processes -- this will serve as some kind of a 'padding'. So, the extra processors will take part in the algorithm as normal for the sake of simplicity, but they will not be doing any useful work.

Total Execution Time: O(log(N)^2) parallel time (best, average, and worst case)