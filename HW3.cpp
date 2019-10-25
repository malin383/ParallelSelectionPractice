// HW3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "tbb/tbb.h"

using namespace tbb;
using namespace std;

//Serial Parallel-prefix sum
/*long* prefix_sum(long* array, long n, long* y, long* s);
long* prefix_sum(long* array, long n) {
	return prefix_sum(array, n, new long[n + 1], new long[2 * (n + 1)]);
}
long* prefix_sum_serial(long* array, long n) {
	long* s = new long[n];
	s[0] = array[0];
	for (long i = 1; i < n; i++)
		s[i] = s[i - 1] + array[i];
	return s;
}*/

//PART 1: PARALLEL PREFIX SUM
long* parallelPrefixSum(long* emptyArr, const long* sumVals, int n){
	long* prefixSummed = new long[n];
	parallel_for(blocked_range<int>(0, n), [=](const blocked_range<int>& r) {
		for (auto i = r.begin(); i < r.end(); i++)
		{
			prefixSummed[i] = parallel_scan(
				blocked_range<int>(0, i+1),
				0,
				[&](const tbb::blocked_range<int>& r, long sum, bool is_final_scan)->int {
					int temp = sum;
					for (int i = r.begin(); i < r.end(); ++i) {
						temp = temp + sumVals[i];
						if (is_final_scan)
							emptyArr[i] = temp;
					}
					return temp;
				},
				[=](long left, long right) {
					return left + right;
				}
				);
		}
	});
	return prefixSummed;
}

//PART 2: PARALLEL PARTITION
long* parallelPartition(long arr[], long partitionVal, int size) {

	//Create copy of array
	long* copy = new long[size];
	for (int i = 0; i < size; ++i)
	{
		copy[i] = arr[i];
	}

	//less than, greater than arrays
	long* lt = new long[size];
	long* gt = new long[size];
	for (int i = 0; i < size; i++)
	{
		lt[i] = (long)(arr[i] < partitionVal);
		gt[i] = (long)(arr[i] > partitionVal);
	}

	//Prefix sum the arrays
	lt = parallelPrefixSum(new long[size], lt, size);
	gt = parallelPrefixSum(new long[size], gt, size);

	//Store k, index of partition val
	long k = lt[size-1];

	//New array with values less than partition value on left and greater than partition val on right
	parallel_for(blocked_range<int>(0, size), [&](const blocked_range<int>& r) {
		for (auto i = r.begin(); i < r.end(); ++i) {
			if (copy[i] < partitionVal) {
				arr[lt[i] - 1] = copy[i];
			}
			else if (copy[i] == partitionVal) {
				arr[k] = partitionVal;
			}
			else {
				arr[k + gt[i]] = copy[i];
			}
		}
		});
	return arr;
}

//PART 3 PARALLEL SELECTION
long parallelSelection(long arr[], long selectVal, int size) {
	//1. Find median of every 5 numbers
	//Use parallel_for
	int mediansNum = size/5 + 1;
	long* medians = new long[mediansNum];	//Store medians in an array

	//Go through each block of 5 and sort in ascending order
	parallel_for(blocked_range<int>(0, mediansNum), [&](blocked_range<int>& r) {
		for (auto i = r.begin(); i < r.end(); ++i)	//i represents block of 5
		{
			int max = (5*i) + 5;
			if (max > size) {
				max = size;
			}
			for (int j = 5 * i; j < max; j++)			//j is 5 * 1 initially and ends at i + 5
			{
				for (int k = j; k < max; k++)			//k iterates through block of 5 to sort
				{
					if (arr[j] > arr[k]) {
						long temp = arr[j];
						arr[j] = arr[k];
						arr[k] = temp;
					}
				}
			}
			medians[i] = arr[(5 * i + max) / 2 - ((5 * i + max + 1) % 2)];
		}
	});

	//DELETE PRINT STATEMENTS WHEN TURNING IN
	for (int i = 0; i < mediansNum; i++)
	{
		cout << medians[i] << " is median\n";
	}
	for (int i = 0; i < size; i++)
	{
		cout << arr[i] << ", ";
	}

	//2. recursively call select to find the median, m, of the middle elements
	if (mediansNum == 1) {
		return medians[0];
	}
	else {
		long m = parallelSelection(medians, selectval, mediansnum);
		//3. partition the whole array around m
		//partition function
		arr = parallelPartition(arr, m, size);
		
		//4. recursively call select(i) on one side of the array
		if (selectVal == k) {
		
		}
		return 0;
	}
	return 0;
}

//Main method
int main()
{
	int size = 12;
	long* z = new long[size]{1, 2, 5, 3, 4, 0, 7, 9, 10, 11, 12, 6};
	long* y = new long[size];
	//long* val = parallelPrefixSum(y, z, 4);
	//cout << "Value is: " << val[2];
	//long* part = parallelPartition(z, 3, 6);
	long sorted = parallelSelection(z, 4, size);
}
