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

#ifdef __linux__
#include <papi.h>
#endif

using namespace std;
using namespace std::chrono;

struct Measurement {
  Measurement(double time)
    : time(time)
  { }

  bool operator<(const Measurement& o) const {
    return time < o.time;
  }

  double time;
};

template <typename Func>
void measure(ostream& out,
             const string& description,
             const size_t trials,
             Func f)
{
  const size_t iMin = 0;
  const size_t iLower = trials / 4;
  const size_t iMedian = trials / 2;
  const size_t iUpper = iLower + iMedian;
  const size_t iMax = trials - 1;
  
  vector<Measurement> measurements;
  
  out << description << "\t" << trials << "\t";
  
  for (unsigned int i = 0; i < trials; i++) {
    auto beginning = high_resolution_clock::now();
    
    // Run the actual test
    f();
               
    high_resolution_clock::duration duration =
    high_resolution_clock::now() - beginning;
    
    double time_spent = duration_cast<milliseconds>(duration).count() / 1000.;

    measurements.push_back(Measurement(time_spent));
  }
  
  // Could be optimized with selection instead of sorting
  sort(measurements.begin(), measurements.end());
  
  out << fixed << measurements[iMin].time << "\t";
  out << fixed << measurements[iLower].time << "\t";
  out << fixed << measurements[iMedian].time << "\t";
  out << fixed << measurements[iUpper].time << "\t";
  out << fixed << measurements[iMax].time << "";
  
  out << endl;
};

random_device rd;
mt19937 generator(100);
uniform_real_distribution<double> distribution(0.0, 10000);
auto random_double = bind(distribution, generator);

template <typename M>
M random_matrix(size_t n, size_t m) {
  M a(n, m);

  for (uint32_t i = 0; i < n; i++) {
    for (uint32_t j = 0; j < m; j++) {
      typename M::Element e(random_double());
      a(0, 0) = e;
    }
  }

  return a;
};

template <typename M0, typename M1, typename Mres>
void test_factor(ostream& out,
                 const size_t trials,
                 size_t factor_pow2,
                 uint64_t min_size_total,
                 uint64_t max_size_total) {
  out << endl << "Test config" << endl;
  out << "A: " << M0::config() << endl;
  out << "B: " << M1::config() << endl;
  out << "C: " << Mres::config() << endl;

  out << "n\tp\tm\tTrials\tMin    [s]\tLower  [s]\tMedian [s]\tUpper  [s]\tMax [s]";
  out << endl;
  int i = factor_pow2 - 1;
  while (true) {
    i++;

    size_t n = (1 << (i - factor_pow2));
    size_t p = 1 << i;
    size_t m = (1 << (i - factor_pow2));

    uint64_t total_size = (uint64_t)n * (uint64_t)p * (uint64_t)m;

    if (total_size < min_size_total) continue;
    if (total_size > max_size_total) break;

    M0 a = random_matrix<M0>(n, p);
    M1 b = random_matrix<M1>(p, m);

    string test = to_string(n) + "\t" + to_string(p) + "\t" + to_string(m);
#ifdef _WINDOWS
    measure(out, test, trials, [&a, &b]() { return a.operator*<M1, Mres>(b); });
#else
    measure(out, test, trials, [&a, &b]() { return a.template operator*<M1, Mres>(b); });
#endif
  }
}

template <typename M0, typename M1, typename Mres>
void test(ostream& out,
          const size_t trials,
          uint64_t min_size_total,
          uint64_t max_size_total) {
  test_factor<M0, M1, Mres>(out, trials, 0, min_size_total, max_size_total);
  test_factor<M0, M1, Mres>(out, trials, 6, min_size_total, max_size_total);
}
