#pragma once

#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <algorithm>

using namespace std;

class BinarySearch {
public:
  static int* arr;
  static size_t n;

  static void preprocess(vector<int>& datapoints);
  static int prev(int q);
  static void cleanup();
};
