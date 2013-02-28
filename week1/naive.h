#pragma once

#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <algorithm>

#include "constants.h"

using namespace std;

template<typename T>
class BinarySearch {
public:
  static T* arr;
  static size_t n;

  static void preprocess(vector<int>& datapoints);
  static int prev(int q);
  static void cleanup();
};

template<typename T>
inline int bs(int q, T* arr, int n)
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

template<typename T>
void BinarySearch<T>::preprocess(vector<int>& datapoints) {
    sort(datapoints.begin(), datapoints.end());
    n = datapoints.size();
    posix_memalign((void**)&arr, CACHE_LINE_SIZE, n * sizeof(T));
    for (int i = 0; i < datapoints.size(); i++)
      arr[i] = T(datapoints[i]);
};

template<typename T>
void BinarySearch<T>::cleanup() {
    // Vector frees itself
};

template<typename T> T* BinarySearch<T>::arr = nullptr;
template<typename T> size_t BinarySearch<T>::n = 0;

template<typename T>
int BinarySearch<T>::prev(int q) {
    return bs(q, arr, n);
};
