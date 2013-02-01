#include <iostream>

#include "blocked.h"
#include "test.h"
#include "naive.h"

using namespace std;

int main(int argc, char* argv[])
{
  cout.precision(3);

  static const int initial_datapoints = 1024 * 1024 / 32;
  static const int max_datapoints = 1024 * 1024 / 32;
  static const int query_count = 10000000 / 100;
  static const int trials = 1; // Same data each time!

  test<BlockedLinear>("B-tree, linear\t", initial_datapoints, max_datapoints, query_count, trials);
  test<BlockedLinearRec>("B-tree, linear rec", initial_datapoints, max_datapoints, query_count, trials);
  test<BlockedBinarySearch>("B-tree, bs\t", initial_datapoints, max_datapoints, query_count, trials);
  test<BinarySearch>("Binary search\t", initial_datapoints, max_datapoints, query_count, trials);

  return 0;
}
