#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <vector>
#include <algorithm>

const int B = 7;
const int d = B + 1;

using namespace std;

class Blocked {
public:
  static int* arr;
  static int* numbers;
  static size_t n;

  static void cleanup();
};

class BlockedBFS : public Blocked {
public:
  static void preprocess(vector<int>& datapoints);
};

class BlockedDFS : public Blocked {
public:
  static void preprocess(vector<int>& datapoints);
};

class BlockedDFSLinear : public BlockedDFS {
public:
  static int prev(int q);
};

class BlockedLinear : public BlockedBFS {
public:
  static int prev(int q);
};

class BlockedLinearRec : public BlockedBFS {
public:
  static int prev(int q);
};

class BlockedBinarySearch : public BlockedBFS {
public:
  static int prev(int q);
};