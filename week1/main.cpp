#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// No assertions!!!
#define NDEBUG 1
#include <cassert>

#include "blocked.h"
#include "naive.h"
#include "test.h"
#include "ComparisonCounter.h"

using namespace std;

int sanity_check(int num_datapoints, int num_queries)
{
  auto datapoints = real_random_data(num_datapoints);
  auto queries = real_random_data(num_queries);
  
  auto bs = query<BinarySearch<int>>(datapoints, queries);
  auto blocked_linear = query<BlockedLinear<int>>(datapoints, queries);
  auto blocked_linear_rec = query<BlockedLinearRec<int>>(datapoints, queries);
  auto blocked_bs = query<BlockedBinarySearch<int>>(datapoints, queries);
  auto blocked_dfs_linear = query<BlockedDFSLinear<int>>(datapoints, queries);

  return result_dist(bs, blocked_linear)
    + result_dist(bs, blocked_linear_rec)
    + result_dist(bs, blocked_bs)
    + result_dist(bs, blocked_dfs_linear);
}

template<typename Impl>
void run_test(string name) {
  static const int initial_datapoints = 1;
  static const int max_datapoints = 1024 * 1024 * 32;
  static const int query_count = 10000000;
  static const int trials = 5; // Same data each time!

  stringstream filename;
  filename << "plots/data/" << name << "_" << B << ".dat";
  ofstream out(filename.str(), ofstream::out);

  test<Impl>(out, name, initial_datapoints, max_datapoints, query_count, trials);

  out.close();
}

int main(int argc, char* argv[])
{  
  cout.precision(3);

  int errors = sanity_check(7, 10000);
  if (errors == 0)
    cout << "No errors. All good :D!" << endl;
  else
    cout << "Oh no! We have " << errors << " errors!" << endl;

  run_test<BlockedLinear<ComparisonCounter>>("comps_Btree_lin");
  run_test<BlockedLinearRec<ComparisonCounter>>("comps_Btree_linrec");
  run_test<BlockedBinarySearch<ComparisonCounter>>("comps_Btree_bs");
  run_test<BinarySearch<ComparisonCounter>>("comps_bs");
  run_test<BlockedDFSLinear<ComparisonCounter>>("comps_DFS_lin");

  return 0;
}
