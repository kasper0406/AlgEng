#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <vector>
#include <algorithm>

#include "constants.h"
#include "ComparisonCounter.h"

using namespace std;

const int B = 32;
const int d = B + 1;

using namespace std;


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
    while (B * block < n) {
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
    while (B * block < n) {
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
        if (index != start - 1)
            cur_answer = index;
        block = d * block + 2 + index - start;
    }
    
    return cur_answer;
};


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

template<typename T>
class BlockedLinear : public Blocked<T> {
public:
  static int prev(int q);
};

template<typename T>
class BlockedLinearRec : public Blocked<T> {
public:
  static int prev(int q);
};

template<typename T>
class BlockedBinarySearch : public Blocked<T> {
public:
  static int prev(int q);
};

template<typename T>
void Blocked<T>::preprocess(vector<int>& datapoints) {
    sort(datapoints.begin(), datapoints.end());
    n = (int)pow(d, max(1., ceil(log(datapoints.size()) / log(d))));
    posix_memalign((void**)&arr, CACHE_LINE_SIZE, n * sizeof(T));
    numbers = (T*) malloc(n * sizeof(T));

    // memcpy(numbers, &datapoints[0], datapoints.size() * sizeof(T));
    for (int i = 0; i < datapoints.size(); i++)
      numbers[i] = T(datapoints[i]);

    for (int i = 0; i < n - datapoints.size(); i++) {
      numbers[datapoints.size() + i] = T(numeric_limits<int>::max());
    }
    
    build(arr, 0, numbers, 0, n - 2);
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

template<typename T> T* Blocked<T>::arr = nullptr;
template<typename T> T* Blocked<T>::numbers = nullptr;
template<typename T> size_t Blocked<T>::n = 0;
