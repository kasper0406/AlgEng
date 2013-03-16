#pragma once

#include <cstddef>
#include <cassert>
#include <memory>
#include <string>
#include "matrix.h"
#include <iostream>

using namespace std;

class Naive {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());

    Mres c(a.rows(), b.columns());

    for (uint32_t i = 0; i < a.rows(); i++) {
      for (uint32_t j = 0; j < b.columns(); j++) {
        typename Mres::Element e(0);

        for (uint32_t k = 0; k < a.columns(); k++) {
          e += a(i, k) * b(k, j);
        }

        c(i, j) = e;
      }
    }
    
    // Move semantics
    return c;
  };

  static string config() {
    return "naive";
  };
};

template<int B>
class Recursive {
public:
  template <typename M0, typename M1, typename Mres>
    static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());

    Mres c(a.rows(), b.columns(), 0);
    do_multiplication(c, a, b,
                      0, a.rows(), 0, a.columns(),
                      0, b.rows(), 0, b.columns(),
                      0, c.rows(), 0, c.columns());

    return c;
  }

  static string config() {
    return "recursive-" + to_string(B);
  };


private:
  // Assumption c is 0 initialized
  template <typename M0, typename M1, typename Mres>
    static void do_multiplication(Mres& result, const M0& a, const M1& b,
                                  uint32_t a_row_start, uint32_t a_row_stop,
                                  uint32_t a_col_start, uint32_t a_col_stop,
                                  uint32_t b_row_start, uint32_t b_row_stop,
                                  uint32_t b_col_start, uint32_t b_col_stop,
                                  uint32_t res_row_start, uint32_t res_row_stop,
                                  uint32_t res_col_start, uint32_t res_col_stop)
  {
    // Notation from article
    const uint32_t m = a_row_stop - a_row_start;
    const uint32_t n = a_col_stop - a_col_start;
    assert(n == b_row_stop - b_row_start);
    const uint32_t p = b_col_stop - b_col_start;

    assert(m == res_row_stop - res_row_start);
    assert(p == res_col_stop - res_col_start);

    if (m <= B && n <= B && p <= B) {
      // Base case: Do standard multiplication
      for (uint32_t i = res_row_start; i < res_row_stop; i++) {
        for (uint32_t j = res_col_start; j < res_col_stop; j++) {
          // Load result element
          typename Mres::Element e = result(i, j);

          for (uint32_t k = 0; k < n; k++) {
            e += a(i, a_col_start + k) * b(b_row_start + k, j);
          }

          result(i, j) = e;
        }
      }

    } else if (m > p && m >= n) {
      // Split a
      const uint32_t mid = m / 2;
      do_multiplication(result, a, b,
                        a_row_start, a_row_start + mid, a_col_start, a_col_stop,
                        b_row_start, b_row_stop, b_col_start, b_col_stop,
                        res_row_start, res_row_start + mid, res_col_start, res_col_stop);
      do_multiplication(result, a, b,
                        a_row_start + mid, a_row_stop, a_col_start, a_col_stop,
                        b_row_start, b_row_stop, b_col_start, b_col_stop,
                        res_row_start + mid, res_row_stop, res_col_start, res_col_stop);
    } else if (n > max(m, p)) {
      // Split both
      const uint32_t mid = n / 2;
      do_multiplication(result, a, b,
                        a_row_start, a_row_stop, a_col_start, a_col_start + mid,
                        b_row_start, b_row_start + mid, b_col_start, b_col_stop,
                        res_row_start, res_row_stop, res_col_start, res_col_stop);
      do_multiplication(result, a, b,
                        a_row_start, a_row_stop, a_col_start + mid, a_col_stop,
                        b_row_start + mid, b_row_stop, b_col_start, b_col_stop,
                        res_row_start, res_row_stop, res_col_start, res_col_stop);
    } else {
      assert(p >= max(m, n));
      
      // Split b
      const uint32_t mid = p / 2;
      do_multiplication(result, a, b,
                        a_row_start, a_row_stop, a_col_start, a_col_stop,
                        b_row_start, b_row_stop, b_col_start, b_col_start + mid,
                        res_row_start, res_row_stop, res_col_start, res_col_start + mid);
      do_multiplication(result, a, b,
                        a_row_start, a_row_stop, a_col_start, a_col_stop,
                        b_row_start, b_row_stop, b_col_start + mid, b_col_stop,
                        res_row_start, res_row_stop, res_col_start + mid, res_col_stop);

    }
  }
};
