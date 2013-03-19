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
#include "layouts.h"

#ifdef __linux__
#include <papi.h>
#include </ipcm/cpucounters.h>
#endif

using namespace std;
using namespace std::chrono;

struct Measurement {
  Measurement()
  : time(0),
    l2_cache_hits(0),
    l2_cache_misses(0),
    l3_cache_hits(0),
    l3_cache_misses(0),
    instructions_retired(0)
  { }

  bool operator<(const Measurement& o) const {
    return time < o.time;
  }

  double time;

  uint64_t l2_cache_hits;
  uint64_t l2_cache_misses;
  uint64_t l3_cache_hits;
  uint64_t l3_cache_misses;
  uint64_t instructions_retired;
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
    Measurement measurement;

    auto beginning = high_resolution_clock::now();
    
#ifdef __linux__
    SystemCounterState before_sstate = getSystemCounterState();
#endif

    // Run code to be benchmarked
    f();      

#ifdef __linux__
    SystemCounterState after_sstate = getSystemCounterState();
    measurement.l2_cache_hits        = getL2CacheHits(before_sstate, after_sstate);
    measurement.l2_cache_misses      = getL2CacheMisses(before_sstate, after_sstate);
    measurement.l3_cache_hits        = getL3CacheHits(before_sstate, after_sstate);
    measurement.l3_cache_misses      = getL3CacheMisses(before_sstate, after_sstate);
    measurement.instructions_retired = getInstructionsRetired(before_sstate, after_sstate);
#endif

    high_resolution_clock::duration duration =
    high_resolution_clock::now() - beginning;
    
    double time_spent = duration_cast<milliseconds>(duration).count() / 1000.;

    measurement.time = time_spent;
    measurements.push_back(measurement);
  }
  
  // Could be optimized with selection instead of sorting
  sort(measurements.begin(), measurements.end());
  
  out << fixed << measurements[iMin].time << "\t";
  out << fixed << measurements[iLower].time << "\t";
  out << fixed << measurements[iMedian].time << "\t";
  out << fixed << measurements[iUpper].time << "\t";
  out << fixed << measurements[iMax].time << "\t";

#ifdef __linux__
  out << fixed << measurements[iMedian].l2_cache_hits << "\t";
  out << fixed << measurements[iMedian].l2_cache_misses << "\t";
  out << fixed << measurements[iMedian].l3_cache_hits << "\t";
  out << fixed << measurements[iMedian].l3_cache_misses << "\t";
  out << fixed << measurements[iMedian].instructions_retired << "\t";
#endif
  
  out << endl;
};

mt19937 generator(100);
uniform_real_distribution<double> distribution(-1., 1.);
auto random_double = bind(distribution, generator);

template <typename M>
M random_matrix(size_t n, size_t m) {
  M a(n, m);

  for (uint32_t i = 0; i < n; i++) {
    for (uint32_t j = 0; j < m; j++) {
      typename M::Element e(random_double());
      a(i, j) = e;
    }
  }

  return a;
};

#ifndef _WINDOWS
void generate_plot(string outputfile, string data) {
  stringstream ss;

  ss << "log2(x) = log(x)/log(2)" << endl
     << "max(a,b) = (a > b) ? a : b" << endl

     << "set terminal pdf enhanced font \"Helvetica, 10\" size 6,4" << endl
     << "set output \"./plots/" << outputfile << ".pdf\"" << endl

     << "set xlabel \"Input size\"" << endl
     << "set xtics 0, 1" << endl
     << "set offset 1, 1" << endl

     << "set ylabel \"Normalized running time [s]\"" << endl
     << "set y2label \"Cache misses\"" << endl

     << "set ytics nomirror tc lt 1" << endl
     << "set y2tics nomirror tc lt 2" << endl

     << "set logscale y" << endl
     << "set logscale y2" << endl
     << "set grid my2tics" << endl

     << "set key vert left top reverse" << endl
     << "set pointsize 2" << endl

     << "plot '-' using (log2($1*$2*$3)):($7) title \"Time\" axes x1y1 with linespoints, \
              '-' using (log2($1*$2*$3)):($11) title \"L2 cache misses\" axes x1y2 with linespoints, \
              '-' using (log2($1*$2*$3)):($13) title \"L3 cache misses\" axes x1y2 with linespoints" << endl

    << data << endl << "e" << endl << data << endl << "e" << endl << data << endl << "e" << endl;

  FILE *gnuplot = popen("gnuplot", "w");
  fputs(ss.str().c_str(), gnuplot);
  pclose(gnuplot);
}
#endif

template <typename M0, typename M1, typename Mres>
  void print_header(ostream& out, size_t factor_pow2) {
  out << endl << "Test config" << endl
      << "Factor: " << factor_pow2 << endl
      << "A: " << M0::config() << endl
      << "B: " << M1::config() << endl
      << "C: " << Mres::config() << endl
      << "n\tp\tm\tTrials\tMin    [s]\tLower  [s]\tMedian [s]\tUpper  [s]\tMax [s]";
#ifdef __linux__
  out << "\tL2 hits\tL2 misses\tL3 hits\tL3 misses\tInst. ret.";
#endif

  out << endl;
}

template <typename M0, typename M1, typename Mres>
void test_factor(ostream& out,
                 const size_t trials,
                 size_t factor_pow2,
                 uint64_t min_size_total,
                 uint64_t max_size_total) {
  stringstream results;

  print_header<M0, M1, Mres>(results, factor_pow2);
  print_header<M0, M1, Mres>(out, factor_pow2);

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
    stringstream measurement;
#ifdef _WINDOWS
    measure(measurement, test, trials, [&a, &b]() { return a.operator*<M1, Mres>(b); });
#else
    measure(measurement, test, trials, [&a, &b]() { return a.template operator*<M1, Mres>(b); });
#endif

    out << measurement.str();
    results << measurement.str();
  }

#ifndef _WINDOWS
  string name = M0::config() + "_" + M1::config() + "_" + Mres::config() + "_" + to_string(factor_pow2);
  generate_plot(name, results.str());
#endif
}

#ifdef __linux__
PCM *m;
#endif

template <typename M0, typename M1, typename Mres>
void test(ostream& out,
          const size_t trials,
          uint64_t min_size_total,
          uint64_t max_size_total) {
#ifdef __linux__
  m = PCM::getInstance();
  // program counters, and on a failure just exit
  if (m->program() != PCM::Success)
    throw runtime_error("Failed to start PCM.");
#endif

  test_factor<M0, M1, Mres>(out, trials, 0, min_size_total, max_size_total);
  //test_factor<M0, M1, Mres>(out, trials, 6, min_size_total, max_size_total); TODO

#ifdef __linux__
  m->cleanup();
  delete m;
#endif
}

typedef Matrix<RowBased<double>, Naive> RN;

template <typename M0, typename M1>
void sanity_check() {
  size_t factor_pow2 = 0;
  size_t min_size_total = 32768;
  size_t max_size_total = 1024 * 1024 * 8;
  
  int i = factor_pow2 - 1;
  while (true) {
    i++;

    size_t n = (1 << (i - factor_pow2));
    size_t p = 1 << i;
    size_t m = (1 << (i - factor_pow2));

    uint64_t total_size = (uint64_t)n * (uint64_t)p * (uint64_t)m;

    if (total_size < min_size_total) continue;
    if (total_size > max_size_total) break;

    RN a = random_matrix<RN>(n, p);
    RN b = random_matrix<RN>(p, m);
    RN c = a.operator*<RN, RN>(b);

    M0 a0 = a.convert<M0>();
    M1 b0 = b.convert<M1>();
#ifndef _WINDOWS
    M0 c0 = a0.template operator*<M1, M0>(b0);
#else
    M0 c0 = a0.operator*<M1, M0>(b0);
#endif

    if (c.operator!=<M0>(c0)) {
      cout << "Sanity check failed!" << endl;
      cout << "A:" << endl << a.to_string();
      cout << "B:" << endl << b.to_string();
      cout << "C:" << endl << c.to_string();
      cout << "C':" << endl << c0.to_string();
      break;
    }
  }

  cout << "Sanity check completed." << endl;
}