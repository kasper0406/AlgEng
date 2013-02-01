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

#ifdef __LINUX__
#include <papi.h>
#endif

using namespace std;
using namespace std::chrono;

vector<int> unique_random_data(const size_t length);
vector<int> random_data(const size_t length);

struct CPUCounterInfo {
  long long L1_cache_misses;
  long long L2_cache_misses;
  long long branch_mispredictions;
};

template <typename Func>
void measure(const string& description, const size_t trials, Func& f) {
  const size_t iMin = 0;
  const size_t iLower = trials / 4;
  const size_t iMedian = trials / 2;
  const size_t iUpper = iLower + iMedian;
  const size_t iMax = trials - 1;
  
  vector<pair<double, CPUCounterInfo>> measurements;
  
  cout << description << "\t" << trials << "\t";
  
  for (unsigned int i = 0; i < trials; i++) {
    CPUCounterInfo cpu_counts;
    memset(&cpu_counts, 0, sizeof(cpu_counts));
    
#ifdef __LINUX__
    // Setup PAPI events and memory for storing counter values
    int cpu_events[] = { PAPI_L1_DCM, // L1 cache misses
                         PAPI_L2_DCM, // L2 cache misses
                         PAPI_BR_MSP  // Branch mispredictions
                       };
    const int cpu_eventslen = sizeof(events) / sizeof(int);
#endif
    
    auto beginning = high_resolution_clock::now();
    
#ifdef __LINUX__
    // Start PAPI
    if (PAPI_start_counters(cpu_events, cpu_eventslen) != PAPI_OK)
      throw runtime_error("Failed to start PAPI counters!");
#endif
    
    // Run the actual test
    f();
                        
#ifdef __LINUX__
    if (PAPI_stop_counters(&cpu_counts, cpu_eventslen) != PAPI_OK)
      throw runtime_error("Failed to stop PAPI countaers!");
#endif
    
    high_resolution_clock::duration duration =
    high_resolution_clock::now() - beginning;
    
    double time_spent = duration_cast<milliseconds>(duration).count() / 1000.;
    measurements.push_back(make_pair(time_spent, cpu_counts));
  }
  
  // Could be optimized with selection instead of sorting
  sort(measurements.begin(), measurements.end(), [] (const pair<double, CPUCounterInfo>& a,
                                                     const pair<double, CPUCounterInfo>& b)
       {
         return a.first < b.first;
       });
  
  cout << fixed << measurements[iMin].first << "s\t";
  cout << fixed << measurements[iLower].first << "s\t";
  cout << fixed << measurements[iMedian].first << "s\t";
  cout << fixed << measurements[iUpper].first << "s\t";
  cout << fixed << measurements[iMax].first << "s";
  
#ifdef __LINUX__
  cout << "\t";
  
  // TODO: Also report results which are not the median!
  cout << fixed << measurements[iMedian].second.L1_cache_misses << "\t";
  cout << fixed << measurements[iMedian].second.L2_cache_misses << "\t";
  cout << fixed << measurements[iMedian].second.branch_mispredictions;
#endif
  
  cout << endl;
};

template <typename T>
void test(string test,
          const size_t initial_datapoints,
          const size_t max_datapoints,
          const size_t query_count,
          const size_t trials) {
  size_t datapoints_size = initial_datapoints;
  
  cout << "Test\t\t\tSize\tTrials\tMin\tLower\tMedian\tUpper\tMax";
#ifdef __LINUX__
  cout << "\tL1 mis\tL2 mis\tBranch mis";
#endif
  cout << endl;
  
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

template<typename T>
vector<pair<bool,int>> query(vector<int> datapoints, vector<int> queries) {
  vector<pair<bool,int>> results;
  T::preprocess(datapoints);
  for (auto q : queries) {
    int index = T::prev(q);
    if (index != -1)
      results.push_back(make_pair(true, T::arr[index]));
    else
      results.push_back(make_pair(false, 0));
  }
  T::cleanup();
  
  return results;
}

int result_dist(vector<pair<bool,int>> a, vector<pair<bool,int>> b);
