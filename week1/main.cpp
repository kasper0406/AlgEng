#include <iostream>

#include "blocked.h"
#include "naive.h"
#include "test.h"

using namespace std;

int sanity_check(int num_datapoints, int num_queries)
{
  auto datapoints = random_data(num_datapoints);
  auto queries = random_data(num_queries);
  
  auto bs = query<BinarySearch>(datapoints, queries);
  auto blocked_linear = query<BlockedLinear>(datapoints, queries);
  auto blocked_linear_rec = query<BlockedLinearRec>(datapoints, queries);
  auto blocked_bs = query<BlockedBinarySearch>(datapoints, queries);
  auto blocked_dfs_linear = query<BlockedDFSLinear>(datapoints, queries);
  
  // TODO: Der er fejl i BFS! Og inkluder DFS i sanity test

  return result_dist(bs, blocked_linear)
     + result_dist(bs, blocked_linear_rec)
     + result_dist(bs, blocked_bs);/*
     + result_dist(bs, blocked_dfs_linear);*/
}

// TODO: Assert er med i release mode!!?

int main(int argc, char* argv[])
{  
  cout.precision(3);

  int errors = sanity_check(7, 10000);
  if (errors == 0)
    cout << "No errors. All good :D!" << endl;
  else
    cout << "Oh no! We have " << errors << " errors!" << endl;

  static const int initial_datapoints = 1024 * 128;
  static const int max_datapoints = 1024 * 1024 * 8;
  static const int query_count = 200000;
  static const int trials = 1; // Same data each time!

  test<BlockedLinear>("B-tree, linear\t", initial_datapoints, max_datapoints, query_count, trials);
  test<BlockedLinearRec>("B-tree, linear rec", initial_datapoints, max_datapoints, query_count, trials);
  test<BlockedBinarySearch>("B-tree, bs\t", initial_datapoints, max_datapoints, query_count, trials);
  test<BinarySearch>("Binary search\t", initial_datapoints, max_datapoints, query_count, trials);
  test<BlockedDFSLinear>("DFS, linear\t", initial_datapoints, max_datapoints, query_count, trials);

  return 0;
}
