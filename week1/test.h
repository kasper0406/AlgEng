#pragma once

#include <iostream>
#include <fstream>
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
#include <stdexcept>

#include "ComparisonCounter.h"

#ifdef __linux__
#include <papi.h>
#endif

using namespace std;
using namespace std::chrono;

vector<int> unique_random_data(const size_t length);
vector<int> random_data(const size_t length, long long seed);
vector<int> real_random_data(const size_t length);

struct CPUCounterInfo {
  long long L1_cache_misses;
  long long L2_cache_misses;
  long long branch_mispredictions;
  long long tlb_misses;
  long long instructions;
};

struct Measurement {
  Measurement(double time, double comparisons, CPUCounterInfo cpu_counters)
    : time(time), comparisons(comparisons), cpu_counters(cpu_counters)
  { }

  bool operator<(const Measurement& o) const {
    return time < o.time;
  }

  double time;
  double comparisons;
  CPUCounterInfo cpu_counters;
};

template <typename Func>
void measure(ofstream& out,
             const string& description,
             int query_count,
             const size_t trials,
             Func& f)
{
  const size_t iMin = 0;
  const size_t iLower = trials / 4;
  const size_t iMedian = trials / 2;
  const size_t iUpper = iLower + iMedian;
  const size_t iMax = trials - 1;
  
  vector<Measurement> measurements;
  
  out << description << "\t" << trials << "\t";
  
  for (unsigned int i = 0; i < trials; i++) {
    CPUCounterInfo cpu_counts = { 0, 0, 0, 0, 0 };
#ifdef __linux__
    // setup PAPI events and memory for storing counter values
    int cpu_events[] = { PAPI_L1_DCM, // L1 cache misses
                         PAPI_L2_DCM, // L2 cache misses
                         PAPI_BR_MSP, // Branch mispredictions
                         PAPI_TLB_IM, // Translation lookaside buffer misses
                         PAPI_TOT_INS // Instructions completed
                       };

    const int cpu_events_len = sizeof(cpu_events) / sizeof(int);
#endif
   
    auto beginning = high_resolution_clock::now();
    
#ifdef __linux__
    // Start PAPI
    if (PAPI_start_counters(cpu_events, cpu_events_len) != PAPI_OK)
      throw runtime_error("Failed to start PAPI counters!");
#endif
    
    // Run the actual test
    f();
                        
#ifdef __linux__
    if (PAPI_stop_counters((long long*)&cpu_counts, cpu_events_len) != PAPI_OK)
      throw runtime_error("Failed to stop PAPI countaers!");
#endif
    
    high_resolution_clock::duration duration =
    high_resolution_clock::now() - beginning;
    
    double time_spent = duration_cast<milliseconds>(duration).count() / 1000.;

    double comparisons = (double) ComparisonCounter::counter / query_count;
    ComparisonCounter::counter = 0;

    measurements.push_back(Measurement(time_spent, comparisons, cpu_counts));
  }
  
  // Could be optimized with selection instead of sorting
  sort(measurements.begin(), measurements.end());
  
  out << fixed << measurements[iMin].time << "\t";
  out << fixed << measurements[iLower].time << "\t";
  out << fixed << measurements[iMedian].time << "\t";
  out << fixed << measurements[iUpper].time << "\t";
  out << fixed << measurements[iMax].time << "";
  
#ifdef __linux__
  out << "\t";
  
  // TODO: Also report results which are not the median!
  out << fixed << measurements[iMedian].cpu_counters.L1_cache_misses << "\t";
  out << fixed << measurements[iMedian].cpu_counters.L2_cache_misses << "\t";
  out << fixed << measurements[iMedian].cpu_counters.branch_mispredictions << "\t";
  out << fixed << measurements[iMedian].cpu_counters.tlb_misses << "\t";
  out << fixed << measurements[iMedian].cpu_counters.instructions;
#endif

  if (measurements[iMedian].comparisons != 0) {
    out << "\t" << fixed << measurements[iMedian].comparisons;
  } else {
    out << "\t" << fixed << "-";
  }
  
  out << endl;
};

template <typename T>
void test(ofstream& out,
          string test,
          const size_t initial_datapoints,
          const size_t max_datapoints,
          const size_t query_count,
          const size_t trials) {
  size_t datapoints_size = initial_datapoints;

  out << "Test\tSize\tTrials\tMin [s]\tLower [s]\tMedian [s]\tUpper [s]\tMax [s]";
#ifdef __linux__
  out << "\tL1 mis\tL2 mis\tBranch mis\tTLB mis\tInstructions";
#endif
  out << "\tComparisons";
  out << endl;
    
  // Ensure that the queries aren't optimized away
  int result = 0;
  
  while (datapoints_size <= max_datapoints) {
    auto datapoints = random_data(datapoints_size, 0x859DBA58DA85);
    auto queries = random_data(query_count, 0xDD89A4F39B);
    
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
    measure(out, ss.str(), query_count, trials, test_function);
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
