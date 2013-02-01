#include "naive.h"

using namespace std;

inline int bs(int q, int* arr, int n)
{
  int min, max, mid;
  min = 0;
  max = n - 1;

  while (min <= max) {
    mid = (max + min) / 2;
    if (arr[mid] < q)
      min = mid + 1;
    else if (arr[mid] > q)
      max = mid - 1;
    else
      return mid;
  }

  return max;
};

void BinarySearch::preprocess(vector<int>& datapoints) {
  sort(datapoints.begin(), datapoints.end());
  n = datapoints.size();
  arr = &datapoints[0];
};

void BinarySearch::cleanup() {
  // Vector frees itself
};

int* BinarySearch::arr = nullptr;
size_t BinarySearch::n = 0;

int BinarySearch::prev(int q) {
  return bs(q, arr, n);
};