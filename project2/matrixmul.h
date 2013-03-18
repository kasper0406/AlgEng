#pragma once

#include <cstddef>
#include <cassert>
#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <array>
#include <vector>
#include "matrix.h"

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

template <size_t T>
class ParallelNaive {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());

    Mres c(a.rows(), b.columns());

    vector<thread> threads;

    size_t a_rows = a.rows();
    size_t slice = a_rows / T;

    for(int t = 0; t < T; ++t){
      uint32_t from = t * slice;
      uint32_t to = t == T - 1 ? a_rows : (t + 1) * slice;

      auto run = [&a, &b, &c, from, to](){
        for (uint32_t i = from; i < to; i++) {
          for (uint32_t j = 0; j < b.columns(); j++) {
            typename Mres::Element e(0);

            for (uint32_t k = 0; k < a.columns(); k++) {
              e += a(i, k) * b(k, j);
            }

            c(i, j) = e;
          }
        }
      };

      if (t == T - 1) {
        run();
      } else {
        threads.push_back(thread(run));
      }
    }

    for(auto& thread : threads){
      thread.join();
    }
    
    // Move semantics
    return c;
  };

  static string config() {
    return to_string(T) + "parallel naive";
  };
};

class GenericBCMultiplier
{
public:
  template<typename M0, typename M1, typename Mres>
  inline static void multiply(Mres& result, const M0& a, const M1& b,
                       uint32_t a_row_start, uint32_t a_row_stop,
                       uint32_t a_col_start, uint32_t a_col_stop,
                       uint32_t b_row_start, uint32_t b_row_stop,
                       uint32_t b_col_start, uint32_t b_col_stop,
                       uint32_t res_row_start, uint32_t res_row_stop,
                       uint32_t res_col_start, uint32_t res_col_stop)
  {
    const uint32_t n = a_col_stop - a_col_start;
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
  }
};

class TiledBCMultiplier
{
public:
  template<typename M0, typename M1, typename Mres>
  inline static void multiply(Mres& result, const M0& a, const M1& b,
                       uint32_t a_row_start, uint32_t a_row_stop,
                       uint32_t a_col_start, uint32_t a_col_stop,
                       uint32_t b_row_start, uint32_t b_row_stop,
                       uint32_t b_col_start, uint32_t b_col_stop,
                       uint32_t res_row_start, uint32_t res_row_stop,
                       uint32_t res_col_start, uint32_t res_col_stop)
  {
    const size_t C_a = a_col_start / M0::Layout::WIDTH;
    const size_t R_a = a_row_start / M0::Layout::HEIGHT;
    const size_t a_start_index = C_a * M0::Layout::WIDTH * M0::Layout::HEIGHT + R_a * M0::Layout::HEIGHT * a.columns();
    
    const size_t C_b = b_col_start / M1::Layout::WIDTH;
    const size_t R_b = b_row_start / M1::Layout::HEIGHT;
    const size_t b_start_index = C_b * M1::Layout::WIDTH * M1::Layout::HEIGHT + R_b * M1::Layout::HEIGHT * b.columns();
    
    const size_t C_res = res_col_start / Mres::Layout::WIDTH;
    const size_t R_res = res_row_start / Mres::Layout::HEIGHT;
    const size_t res_start_index = C_res * Mres::Layout::WIDTH * Mres::Layout::HEIGHT + R_res * Mres::Layout::HEIGHT * result.columns();
    
    const uint32_t m = a_row_stop - a_row_start;
    const uint32_t n = a_col_stop - a_col_start;
    const uint32_t p = b_col_stop - b_col_start;
    
    // Base case: Do standard multiplication
    for (uint32_t i = 0; i < m; i++) {
      // TODO: Consider how to make this work with arbitrary tiling!!!
      const size_t a_start_row = a_start_index + i * M0::Layout::WIDTH;
      const size_t res_start_row = res_start_index + i * Mres::Layout::WIDTH;
      
      for (uint32_t j = 0; j < p; j++) {
        // Load result element
        typename Mres::Element& e = result.at(res_start_row + j);
        
        for (uint32_t k = 0; k < n; k++) {
          // e += a(i, a_col_start + k) * b(b_row_start + k, j);
          e += a.at(a_start_row + k) * b.at(b_start_index + j * M1::Layout::HEIGHT + k);
        }
        
        // result.at(res_start_row + j) = e;
      }
    }
  }
};

template<int B>
class ZLayoutBCMultiplier
{
public:
  template<typename M0, typename M1, typename Mres>
  inline static void multiply(Mres& result, const M0& a, const M1& b,
                              uint32_t a_row_start, uint32_t a_row_stop,
                              uint32_t a_col_start, uint32_t a_col_stop,
                              uint32_t b_row_start, uint32_t b_row_stop,
                              uint32_t b_col_start, uint32_t b_col_stop,
                              uint32_t res_row_start, uint32_t res_row_stop,
                              uint32_t res_col_start, uint32_t res_col_stop)
  {
    assert(a_row_stop - a_row_start == B &&
           a_col_stop - a_col_start == B &&
           b_col_stop - b_col_start == B);
    
    const size_t start_a = ZCurve<int>::interleave_bits(a_row_start, a_col_start);
    const size_t start_b = ZCurve<int>::interleave_bits(b_row_start, b_col_start);
    const size_t start_res = ZCurve<int>::interleave_bits(res_row_start, res_col_start);
    
    for (uint32_t i = 0; i < B; i++) {
      for (uint32_t j = 0; j < B; j++) {
        // Load result element
        typename Mres::Element& e = result.at(start_res + offsets[i * B + j]);
        
        for (uint32_t k = 0; k < B; k++) {
          // e += a(i, a_col_start + k) * b(b_row_start + k, j);
          e += a.at(start_a + offsets[i * B + k]) * b.at(start_b + offsets[k * B + j]);
        }
      }
    }
  }
  
private:
  static array<int, B * B> makeIndexes() {
    array<int, B * B> indexes;
    
    for (int i = 0; i < B; i++) {
      for (int j = 0; j < B; j++)
        indexes[i * B + j] = ZCurve<int>::interleave_bits(i, j);
    }
    return indexes;
  }
  
  const static array<int, B * B> offsets;
};
template<int B> const array<int, B * B> ZLayoutBCMultiplier<B>::offsets = ZLayoutBCMultiplier<B>::makeIndexes();

// TODO: Assumes equal columns and rows! and base of two!
template<int B, typename BaseCaseMultiplier>
class Strassen {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());

    size_t n = a.rows();
    size_t p = a.columns();
    size_t m = b.columns();

    if (m <= B && n <= B && p <= B) {
      Mres c(a.rows(), b.columns(), 0); // TODO: Init necessary?
#ifndef _WINDOWS
      BaseCaseMultiplier::template multiply<M0, M1, Mres>(c, a, b,
                                                          0, a.rows(), 0, a.columns(),
                                                          0, b.rows(), 0, b.columns(),
                                                          0, a.rows(), 0, b.columns());
#else
      BaseCaseMultiplier::multiply<M0, M1, Mres>(c, a, b,
                                                 0, a.rows(), 0, a.columns(),
                                                 0, b.rows(), 0, b.columns(),
                                                 0, a.rows(), 0, b.columns());
#endif

      return c;
    } else {
      size_t new_n = n / 2;
      size_t new_p = p / 2;
      size_t new_m = m / 2;

      M0 a11(a, 0, new_n, 0, new_p);
      M0 a12(a, new_n, n, 0, new_p);
      M0 a21(a, 0, new_n, new_p, p);
      M0 a22(a, new_n, n, new_p, p);

      M1 b11(a, 0, new_p, 0, new_m);
      M1 b12(a, new_p, p, 0, new_m);
      M1 b21(a, 0, new_p, new_m, m);
      M1 b22(a, new_p, p, new_m, m);

      Mres c11 = a11.operator*<M0, M1>(b11).operator+<Mres, Mres>(a12.operator*<M0, M1>(b21));
      Mres c12 = a11.operator*<M0, M1>(b12).operator+<Mres, Mres>(a12.operator*<M0, M1>(b22));
      Mres c21 = a21.operator*<M0, M1>(b11).operator+<Mres, Mres>(a22.operator*<M0, M1>(b21));
      Mres c22 = a21.operator*<M0, M1>(b12).operator+<Mres, Mres>(a22.operator*<M0, M1>(b22));

      return Mres(c11, c12, c21, c22);
    }
  };

  static string config() {
    return "strassen-" + to_string(B);
  };
};

template<int B, typename BaseCaseMultiplier>
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
#ifndef _WINDOWS
      BaseCaseMultiplier::template multiply<M0, M1, Mres>(result, a, b,
                                                          a_row_start, a_row_stop, a_col_start, a_col_stop,
                                                          b_row_start, b_row_stop, b_col_start, b_col_stop,
                                                          res_row_start, res_row_stop, res_col_start, res_col_stop);
#else
      BaseCaseMultiplier::multiply<M0, M1, Mres>(result, a, b,
                                                 a_row_start, a_row_stop, a_col_start, a_col_stop,
                                                 b_row_start, b_row_stop, b_col_start, b_col_stop,
                                                 res_row_start, res_row_stop, res_col_start, res_col_stop);
#endif
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
