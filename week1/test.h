#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <ctime>
#include <string>
#include <sstream>
#include <set>
#include <limits>
#include <functional>
#include <sstream>
#include <climits>
#include <algorithm>
#include <chrono>
#include <random>
#include <cassert>

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

  for (unsigned int i = 0; i < trials; i++) {
    auto beginning = high_resolution_clock::now();
  
    f();

    high_resolution_clock::duration duration =
      high_resolution_clock::now() - beginning;

    measurements.push_back(
      duration_cast<milliseconds>(duration).count() / 1000.);
  }

  // Could be optimized with selection instead of sorting
  sort(measurements.begin(), measurements.end());

  cout << fixed << measurements[iMin] << "s\t";
  cout << fixed << measurements[iLower] << "s\t";
  cout << fixed << measurements[iMedian] << "s\t";
  cout << fixed << measurements[iUpper] << "s\t";
  cout << fixed << measurements[iMax] << "s" << endl;
};

template <typename T>
void test(string test,
          const size_t initial_datapoints,
          const size_t max_datapoints, 
          const size_t query_count,
          const size_t trials) {
  size_t datapoints_size = initial_datapoints;

  cout << "Test\t\t\tSize\tTrials\tMin\tLower\tMedian\tUpper\tMax" << endl;

  // Ensure that the queries aren't optimized away
  int result = 0;

  while (datapoints_size <= max_datapoints) {
    auto datapoints = random_data(datapoints_size);
    auto queries = random_data(query_count);

    function<void ()> test_function = [datapoints, queries, &result]() -> void {
      int v = 0;

      for (unsigned int i = 0; i < queries.size(); i++) {
        int prev = T::prev(queries[i]);

        v ^= prev;

        // Sanity check
        assert(prev == -1 || T::arr[prev] <= queries[i]);
      }

      result = v;
    };

    stringstream ss;
    ss << test << "\t" << datapoints_size;

    T::preprocess(datapoints);
    measure(ss.str(), trials, test_function);
    T::cleanup();

    datapoints_size *= 2;
  }

  cout << "Testing completed with result " << result << endl;
};