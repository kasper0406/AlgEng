#include <iostream>

#include "blocked.h"
#include "naive.h"
#include "test.h"
#include "ComparisonCounter.h"

using namespace std;

int sanity_check(int num_datapoints, int num_queries)
{
  auto datapoints = random_data(num_datapoints);
  auto queries = random_data(num_queries);
  
  auto bs = query<BinarySearch<int>>(datapoints, queries);
  auto blocked_linear = query<BlockedLinear<int>>(datapoints, queries);
  auto blocked_linear_rec = query<BlockedLinearRec<int>>(datapoints, queries);
  auto blocked_bs = query<BlockedBinarySearch<int>>(datapoints, queries);
  
  return result_dist(bs, blocked_linear)
       + result_dist(bs, blocked_linear_rec)
       + result_dist(bs, blocked_bs);
}

int main(int argc, char* argv[])
{  
  cout.precision(3);

  static const int initial_datapoints = 1;
  static const int max_datapoints = 1024 * 256 * 1;
  static const int query_count = 1000000;
  static const int trials = 5; // Same data each time!
    
  test<BlockedLinear<ComparisonCounter>>("B-tree, linear\t", initial_datapoints, max_datapoints, query_count, trials);
  test<BlockedLinearRec<ComparisonCounter>>("B-tree, linear rec", initial_datapoints, max_datapoints, query_count, trials);
  test<BlockedBinarySearch<ComparisonCounter>>("B-tree, bs\t", initial_datapoints, max_datapoints, query_count, trials);
  test<BinarySearch<ComparisonCounter>>("Binary search\t", initial_datapoints, max_datapoints, query_count, trials);
  
  /*
  int errors = sanity_check(1000000, 10000000);
  if (errors == 0)
    cout << "No errors. All good :D!" << endl;
  else
    cout << "Oh no! We have " << errors << " errors!" << endl;
   */

  return 0;
}
