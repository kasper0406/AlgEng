#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <vector>
#include <algorithm>

const int B = 32;
const int d = B + 1;

using namespace std;

class Blocked {
public:
  static int* arr;
  static int* numbers;
  static size_t n;

  static void preprocess(vector<int>& datapoints);
  static int prev(int q);
  static void cleanup();
};

class BlockedLinear : public Blocked {
public:
  static int prev(int q);
};

class BlockedLinearRec : public Blocked {
public:
  static int prev(int q);
};

class BlockedBinarySearch : public Blocked {
public:
  static int prev(int q);
};