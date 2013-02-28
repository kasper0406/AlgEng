#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <vector>
#include <algorithm>
#include <cassert>

#include "constants.h"
#include "ComparisonCounter.h"

const int B = 32;
const int d = B + 1;

using namespace std;

template<typename T>
class Blocked {
public:
  static T* arr;
  static T* numbers;
  static size_t n;

  static void preprocess(vector<int>& datapoints);
  static int prev(int q);
  static void cleanup();
};

template<typename T> T* Blocked<T>::arr = nullptr;
template<typename T> T* Blocked<T>::numbers = nullptr;
template<typename T> size_t Blocked<T>::n = 0;

template<typename T>
class BlockedBFS : public Blocked<T> {
public:
  static void preprocess(vector<int>& datapoints);
};

template<typename T>
class BlockedDFS : public Blocked<T> {
public:
  static void preprocess(vector<int>& datapoints);
};

template<typename T>
class BlockedLinear : public BlockedBFS<T> {
public:
  static int prev(int q);
};

template<typename T>
class BlockedLinearRec : public BlockedBFS<T> {
public:
  static int prev(int q);
};

template<typename T>
class BlockedBinarySearch : public BlockedBFS<T> {
public:
  static int prev(int q);
};

template<typename T>
class BlockedDFSLinear : public BlockedDFS<T> {
public:
  static int prev(int q);
};


template<typename T>
void build(T* arr, int pos, T* numbers, int start, int end)
{
    // printf("build(arr, %d, numbers, %d, %d)\n", pos, start, end);
    if (start > end) return; // Should not happen...
    if (start + B - 1 == end) {
        for (int i = 0; i < B; i++)
            arr[pos + i] = numbers[start + i];
        return;
    }
    
    const int parentleaf = pos / B;
    // printf("parent leaf = %d\n", parentleaf);
    
    int prevIndex = start - 1;
    int curleaf;
    for (int i = 1; i < d; i++) {
        curleaf = d * parentleaf + i;
        // printf("cur leaf = %d\n", curleaf);
        
        int index = (int)(start + ((double)(end - start + 1) / d) * i);
        
        // printf("index = %d\n", index);
        arr[B * parentleaf + (i - 1)] = numbers[index];
        build(arr, B * curleaf, numbers, prevIndex + 1, index - 1);
        
        prevIndex = index;
    }
    
    curleaf = d * (parentleaf + 1);
    // printf("cur leaf = %d\n", curleaf);
    build(arr, B * curleaf, numbers, prevIndex + 1, end);
}

template<typename T>
void print_block(T* arr, int block)
{
    if (block == 0)
        printf("1st block:\n");
    else
        printf("%dth block:\n", block + 1);
    
    for (int i = 0; i < B; i++)
        printf("%10d", arr[block * B + i]);
    printf("\n");
}

template<typename T>
int bs_scan_search_rec(int q, T* arr, int n, int block)
{
    // printf("bs_scan_search_rec(%d, arr, %d);\n", q, block);
    // print_block(arr, block);
    
    for (int i = 0; i < B; i++) {
        const int index = B * block + i;
        if (arr[index] > q) {
            int newblock = d * block + 1 + i;
            if (newblock * B >= n) {
                return (i > 0) ? index - 1 : -1;
            } else {
                int res_index = bs_scan_search_rec(q, arr, n, newblock);
                if (res_index != -1)
                    return res_index;
                else
                    return (i > 0) ? index - 1 : -1;
            }
        } else if (arr[index] < q) {
            if (i == B - 1) {
                // Check if we are out of bounds
                int newblock = d * (block + 1);
                if (newblock * B >= n) {
                    return index;
                } else {
                    int res_index = bs_scan_search_rec(q, arr, n, newblock);
                    if (res_index != -1)
                        return res_index;
                    else
                        return index;
                }
            }
        } else {
            // equal
            return index;
        }
    }
    return -1;
}

template<typename T>
int bs_scan_search_iter(int q, T* arr, int n)
{
    int block = 0;
    int cur_answer = -1;
    
    // While we are not out of bounds we search the block
    while (B * block + 1 < n) {
        // Make a linear scan of the block
        for (int i = 0; i < B; i++) {
            const int index = B * block + i;
            if (q < arr[index]) {
                if (i > 0)
                    cur_answer = index - 1;
                block = d * block + 1 + i;
                break;
            } else if (arr[index] < q) {
                cur_answer = index;
                if (i == B - 1) {
                    block = d * (block + 1);
                    break;
                }
            } else {
                // equal
                return index;
            }
        }
    }
    
    return cur_answer;
}

template<typename T>
inline int bs(int q, T* arr, int min, int max)
{
    // printf("bs(%d, arr, %d, %d);\n", q, min, max);
    while (min <= max) {
        const int mid = (max + min) / 2;
        if (arr[mid] < q)
            min = mid + 1;
        else if (arr[mid] > q)
            max = mid - 1;
        else
            return mid;
    }
    return max;
}

template<typename T>
int bs_bs_search_iter(int q, T* arr, int n)
{
    int block = 0;
    // printf("Searching for %d\n", q);
    int cur_answer = -1;

    // While we are not out of bounds we search the block
    while (B * block + 1 < n) {
        // print_block(arr, block);
        
        // Find Pred(q) while only searching the block using BS
        const int start = B * block;
        const int end = B * (block + 1) - 1;
        const int index = bs(q, arr, start, end);
        
        // printf("start = %d, end = %d\n", start, end);
        
        //for (int i = B * block; i < B * (block + 1); i++)
        //  printf("%5d|", arr[i]);
        //printf("\n");
        
        // printf("index = %d\n", index);
        if (index != start - 1) {
          cur_answer = index;
        }
        block = d * block + 2 + index - start;
    }

    return cur_answer;
};

template<typename T>
void build_dfs(T* arr, T* numbers, int n)
{
  int subtree_size = (n - B) / d;

  for (int i = 0; i < B; i++) {
    assert(BlockedDFSLinear<T>::arr + BlockedDFSLinear<T>::n > arr + i);
    arr[i] = numbers[subtree_size + (subtree_size + 1) * i];
  }

  if (subtree_size == 0) return;

  for (int i = 0; i < d; i++) {
    T* arr_start = arr + B + i * subtree_size;
    T* numbers_start = numbers + i * (subtree_size + 1);
    build_dfs(arr_start, numbers_start, subtree_size);
  }
}

template<typename T>
int dfs_linear_scan(int q, T* arr, int n)
{
  T* cur_arr = arr;
  int cur_answer = -1;
  int next_n = n;

  // While we are not coming from a leaf we continue...
  while (next_n != 0) {
    // Subtree_size
    next_n = (next_n - B) / d;

    // Make a linear scan of the block
    for (int i = 0; i < B; i++) {
      const int index = cur_arr - arr + i;
      const T value = cur_arr[i];
      
      if (q < value) {
        // i'te pointer
        cur_arr += B + i * next_n;
        
        break;
      } else if (value < q) {

        cur_answer = index;

        if (i == B - 1) {

          // Rightmost pointer
          cur_arr += B + B * next_n;

          break;
        }
      } else {
        // equal
        return index;
      }
    }
  }

  return cur_answer;
}


template<typename T>
void BlockedBFS<T>::preprocess(vector<int>& datapoints) {
  sort(datapoints.begin(), datapoints.end());
  BlockedBFS<T>::n = (int)pow(d, max(1., ceil(log(datapoints.size()) / log(d))));
  posix_memalign((void**)&BlockedBFS<T>::arr, CACHE_LINE_SIZE, BlockedBFS<T>::n * sizeof(T));
  BlockedBFS<T>::numbers = (T*) malloc(BlockedBFS<T>::n * sizeof(T));

  // memcpy(numbers, &datapoints[0], datapoints.size() * sizeof(T));
  for (int i = 0; i < datapoints.size(); i++)
    BlockedBFS<T>::numbers[i] = T(datapoints[i]);

  for (int i = 0; i < BlockedBFS<T>::n - datapoints.size(); i++) {
    BlockedBFS<T>::numbers[datapoints.size() + i] = T(numeric_limits<int>::max());
  }
    
  build(BlockedBFS<T>::arr, 0, BlockedBFS<T>::numbers, 0, BlockedBFS<T>::n - 2);
  ComparisonCounter::counter = 0;
};

template<typename T>
void BlockedDFS<T>::preprocess(vector<int>& datapoints) {
  sort(datapoints.begin(), datapoints.end());

  BlockedDFS<T>::n = (int)pow(d, max(1., ceil(log(datapoints.size() + 1) / log(d)))) - 1;

  posix_memalign((void**)&BlockedDFS<T>::arr, CACHE_LINE_SIZE, BlockedDFS<T>::n * sizeof(T));
  BlockedDFS<T>::numbers = (T*) malloc(BlockedDFS<T>::n * sizeof(T));
  // memcpy(numbers, &datapoints[0], datapoints.size() * sizeof(int));
  for (int i = 0; i < datapoints.size(); i++)
    BlockedDFS<T>::numbers[i] = T(datapoints[i]);

  for (int i = 0; i < BlockedDFS<T>::n - datapoints.size(); i++) {
    BlockedDFS<T>::numbers[datapoints.size() + i] = T(numeric_limits<int>::max());
  }

  build_dfs(BlockedDFS<T>::arr, BlockedDFS<T>::numbers, BlockedDFS<T>::n);
  ComparisonCounter::counter = 0;
};

template<typename T>
void Blocked<T>::cleanup() {
    free(arr);
    free(numbers);
};

template<typename T>
int BlockedLinear<T>::prev(int q) {
    return bs_scan_search_iter(q, Blocked<T>::arr, Blocked<T>::n);
};

template<typename T>
int BlockedLinearRec<T>::prev(int q) {
    return bs_scan_search_rec(q, Blocked<T>::arr, Blocked<T>::n, 0);
};

template<typename T>
int BlockedBinarySearch<T>::prev(int q) {
    return bs_bs_search_iter(q, Blocked<T>::arr, Blocked<T>::n);
};

template<typename T>
int BlockedDFSLinear<T>::prev(int q) {
  return dfs_linear_scan(q, Blocked<T>::arr, Blocked<T>::n);
};

