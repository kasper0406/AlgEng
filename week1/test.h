#pragma once

#include <vector>
#include <unordered_set>
#include <ctime>
#include <string>
#include <set>
#include <limits>
#include <functional>
#include <sstream>
#include <climits>
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

vector<int> random_data(const size_t length);

template <typename Func>
void measure(const string& description, const size_t trials, Func& f) {
  const size_t iMin = 0;
  const size_t iLower = trials / 4;
  const size_t iMedian = trials / 2;
  const size_t iUpper = iLower + iMedian;
  const size_t iMax = trials - 1;

  vector<double> measurements;

  cout << description << "\t" << trials << "\t";

  high_resolution_clock hclock;

  for (unsigned int i = 0; i < trials; i++) {
    auto beginning = hclock.now()
  
    f();

    high_resolution_clock::duration duration = hclock.now() - beginning;
    
    measurements.push_back(duration_cast<milliseconds>(duration).count());
  }
  
  // Could be optimized with selection instead of sorting
  sort(measurements.begin(), measurements.end());

  cout << fixed << measurements[iMin] << "s\t";
  cout << fixed << measurements[iLower] << "s\t";
  cout << fixed << measurements[iMedian] << "s\t";
  cout << fixed << measurements[iUpper] << "s\t";
  cout << fixed << measurements[iMax] << "s" << endl;
};

template <typename B, typename P>
void test(const size_t initial_datapoints,
          const size_t max_datapoints, 
          const size_t queries_size) {
  size_t datapoints_size = initial_datapoints;

  // Ensure that the queries aren't optimized away
  int result = 0;

  while (datapoints_size <= max_datapoints) {
    auto datapoints = random_data(datapoint_count);
    auto queries = random_data(query_count);


    auto test_function = [datapoints, queries, &result]() -> void {
      B(datapoints);

      int v = 0;

      // TODO: Make it C'ish?
      for (int i = 0; i < queries.size(); i++) {
        v ^= P(queries[i]);
      }

      result = v;
    };

    measure("foobar", 10, test_function);
  }

  cout << "Testing completed with result " << result << endl;
};