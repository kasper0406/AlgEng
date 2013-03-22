#pragma once

#include <cstddef>
#include <cassert>
#include <memory>
#include <functional>
#include <string>
#include <iostream>
#include <array>
#include <thread>
#include <vector>
#include <chrono>
#include <future>
#include "matrix.h"

// AVX intrinsics
#include <immintrin.h>

using namespace std;
using namespace std::chrono;

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
    return to_string(T) + "par-naive";
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
  
  static string config() {
    return "generic-bc";
  };
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
      for (uint32_t j = 0; j < p; j++) {
        // TODO: Consider how to make this work with arbitrary tiling!!!
        const size_t a_start_row = a_start_index + i * M0::Layout::WIDTH;
        const size_t res_start_row = res_start_index + i * Mres::Layout::WIDTH;
        const size_t b_start_row = b_start_index + j * M1::Layout::HEIGHT;
        
        // Load result element
        typename Mres::Element& e = result.at(res_start_row + j);
        
        for (uint32_t k = 0; k < n; k++) {
          // e += a(i, a_col_start + k) * b(b_row_start + k, j);
          e += a.at(a_start_row + k) * b.at(b_start_row + k);
        }
        
        // result.at(res_start_row + j) = e;
      }
    }
  }
  
  static string config() {
    return "tiled-bc";
  };
};

class SIMDTiledBCMultiplier
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
    static_assert(is_same<double, typename Mres::Element>::value &&
                  is_same<double, typename M0::Element>::value &&
                  is_same<double, typename M1::Element>::value,
                  "double type required for SIMD!");
    
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
    
    assert(n % 4 == 0);
    
    // Allocate memory to sum result
    // TODO: Ensure that this is 32 byte aligned!!!
    double tmp[4] = { 0, 0, 0, 0 };
    
    // Base case: Do standard multiplication
    for (uint32_t i = 0; i < m; i++) {
      for (uint32_t j = 0; j < p; j++) {
        // TODO: Consider how to make this work with arbitrary tiling!!!
        const size_t a_start_row = a_start_index + i * M0::Layout::WIDTH;
        const size_t res_start_row = res_start_index + i * Mres::Layout::WIDTH;
        const size_t b_start_row = b_start_index + j * M1::Layout::HEIGHT;
        
        __m256d sum = _mm256_setzero_pd();
        for (uint32_t k = 0; k < n; k += 4) {
          __m256d a_data = _mm256_load_pd(a.addr(a_start_row + k));
          __m256d b_data = _mm256_load_pd(b.addr(b_start_row + k));
          
          __m256d multiplied = _mm256_mul_pd(a_data, b_data);
          sum = _mm256_add_pd(sum, multiplied);
        }

        _mm256_store_pd(tmp, sum);
        
        typename Mres::Element& e = result.at(res_start_row + j);
        for (int h = 0; h < 4; h++)
          e += tmp[h];
      }
    }
  }
  
  static string config() {
    return "simd-tiled-bc";
  };
};

template<int B>
class FixedTiledBCMultiplier
{
public:
  template<typename Mres, typename M0, typename M1>
  inline static Mres multiply(const M0& a, const M1& b)
  {
    assert(a.columns() == b.rows());
    assert(a.columns() == B);

    Mres c(a.rows(), b.columns());

    for (uint32_t i = 0; i < B; i++) {
      for (uint32_t j = 0; j < B; j++) {
        typename Mres::Element e(0);

        for (uint32_t k = 0; k < B; k++) {
          e += a.data.data[i * B + k] * b.data.data[j * B + k];
        }

        c.data.data[i * B + j] = e;
      }
    }
    
    // Move semantics
    return c;
  }
  
  static string config() {
    return to_string(B) + "-fixed-tiled-bc";
  };
};

template<int B>
class FixedTiledBCMultiplier2
{
public:
  template<typename Mres, typename M0, typename M1>
  inline static void multiply(Mres& c, const M0& a, const M1& b)
  {
    assert(a.columns() == b.rows());
    assert(a.columns() == B);

    // Er fikset
    for (uint32_t i = 0; i < B; i++) {
      for (uint32_t j = 0; j < B; j++) {
        typename Mres::Element e(0);

        for (uint32_t k = 0; k < B; k++) {
          e += a.data.data[i * B + k] * b.data.data[j * B + k];
        }

        c.data.data[i * B + j] = e;
      }
    }
  }
  
  static string config() {
    return to_string(B) + "-fixed-tiled-bc2";
  };
};

template<int B>
class SIMDFixedTiledBCMultiplier
{
public:
  template<typename Mres, typename M0, typename M1>
  inline static Mres multiply(const M0& a, const M1& b)
  {
    static_assert(is_same<double, typename Mres::Element>::value &&
                  is_same<double, typename M0::Element>::value &&
                  is_same<double, typename M1::Element>::value,
                  "double type required for SIMD!");
    static_assert(B % 4 == 0, "Base case size should be a multiple of 4!");
    assert(a.columns() == b.rows());
    assert(a.columns() == B);
    
    Mres c(a.rows(), b.columns());
    
    // Allocate memory to sum result
    // TODO: Ensure that this is 32 byte aligned!!!
    double tmp[4] = { 0, 0, 0, 0 };
    
    for (uint32_t i = 0; i < B; i++) {
      for (uint32_t j = 0; j < B; j++) {
        __m256d sum = _mm256_setzero_pd();
        
        typename Mres::Element e(0);
        
        for (uint32_t k = 0; k < B; k += 4) {
          __m256d a_data = _mm256_load_pd(a.addr(i * B + k));
          __m256d b_data = _mm256_load_pd(b.addr(j * B + k));
          
          __m256d multiplied = _mm256_mul_pd(a_data, b_data);
          sum = _mm256_add_pd(sum, multiplied);
        }
        
        _mm256_store_pd(tmp, sum);
        for (int h = 0; h < 4; h++)
          e += tmp[h];
        
        c.data.data[i * B + j] = e;
      }
    }
    
    // Move semantics
    return c;
  }
  
  static string config() {
    return to_string(B) + "-simd-fixed-tiled-bc";
  };
};

template<int B>
class SIMDFixedTiledBCMultiplier2
{
public:
  template<typename Mres, typename M0, typename M1>
  inline static void multiply(Mres& c, const M0& a, const M1& b)
  {
    static_assert(is_same<double, typename Mres::Element>::value &&
                  is_same<double, typename M0::Element>::value &&
                  is_same<double, typename M1::Element>::value,
                  "double type required for SIMD!");
    static_assert(B % 4 == 0, "Base case size should be a multiple of 4!");
    assert(a.columns() == b.rows());
    assert(a.columns() == B);
    
    // Allocate memory to sum result
    // TODO: Ensure that this is 32 byte aligned!!!
    double tmp[4] = { 0, 0, 0, 0 };
    
    for (uint32_t i = 0; i < B; i++) {
        for (uint32_t j = 0; j < B; j++) {
        __m256d sum = _mm256_setzero_pd();
        
        typename Mres::Element e(0);
        
        for (uint32_t k = 0; k < B; k += 4) {
          __m256d a_data = _mm256_load_pd(a.addr(i * B + k));
          __m256d b_data = _mm256_load_pd(b.addr(j * B + k));
          
          __m256d multiplied = _mm256_mul_pd(a_data, b_data);
          sum = _mm256_add_pd(sum, multiplied);
        }
        
        _mm256_store_pd(tmp, sum);
        for (int h = 0; h < 4; h++)
          e += tmp[h];
        
        c.data.data[i * B + j] = e;
      }
    }
  }
  
  static string config() {
    return to_string(B) + "-simd-fixed-tiled-bc2";
  };
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
        
        for (uint32_t k = 0; k < B; k++)
          e += a.at(start_a + offsets[i * B + k]) * b.at(start_b + offsets[k * B + j]);
      }
    }
  }
  
  static string config() {
    return to_string(B) + "-zlayout-bc";
  };
  
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

// TODO: Assumes equal columns and rows! and base of two! Assumes Z-curve tiling
template<int B, typename BaseMul>
class HackyStrassen {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());

    size_t n = a.rows();
    size_t p = a.columns();
    size_t m = b.columns();

    if (m <= B && n <= B && p <= B) {
      return BaseMul::Template multiply<Mres, M0, M1>(a, b);
    } else {
      auto a_split = a.split();
      M0 a11 = move(get<0>(a_split));
      M0 a12 = move(get<1>(a_split));
      M0 a21 = move(get<2>(a_split));
      M0 a22 = move(get<3>(a_split));

      auto b_split = b.split();
      M1 b11 = move(get<0>(b_split));
      M1 b12 = move(get<1>(b_split));
      M1 b21 = move(get<2>(b_split));
      M1 b22 = move(get<3>(b_split));

      Mres m1 = a11.unsafe_add(a22).Template operator*<M1, Mres>(b11.unsafe_add(b22));
      Mres m2 = a21.unsafe_add(a22).Template operator*<M1, Mres>(b11);
      Mres m3 = a11.Template operator*<M1, Mres>(b12.unsafe_sub(b22));
      Mres m4 = a22.Template operator*<M1, Mres>(b21.unsafe_sub(b11));
      Mres m5 = a11.unsafe_add(a12).Template operator*<M1, Mres>(b22);
      Mres m6 = a21.unsafe_sub(a11).Template operator*<M1, Mres>(b11.unsafe_add(b12));
      Mres m7 = a12.unsafe_sub(a22).Template operator*<M1, Mres>(b21.unsafe_add(b22));
      
      Mres c11 = m1.unsafe_add(m4).unsafe_sub(m5).unsafe_add(m7);
      Mres c12 = m3.unsafe_add(m5);
      Mres c21 = m2.unsafe_add(m4);
      Mres c22 = m1.unsafe_sub(m2).unsafe_add(m3).unsafe_add(m6);

      return unsafe_combine<Mres>(c11, c12, c21, c22);
    }
  };

  template <typename Mres>
  static Mres unsafe_combine(const Mres& a11, const Mres& a12, const Mres& a21, const Mres& a22) {
    Mres m(a11.rows() + a21.rows(), a11.columns() + a21.columns());

    uint32_t current = 0;
    uint32_t elements = a11.rows() * a11.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a11.data.data[i];
    }

    elements = a21.rows() * a21.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a21.data.data[i];
    }

    elements = a12.rows() * a12.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a12.data.data[i];
    }

    elements = a22.rows() * a22.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a22.data.data[i];
    }

    return m;
  };

  static string config() {
    return "strassen-" + to_string(B) + "(" + BaseMul::config() + ")";
  };
};

// TODO: Assumes equal columns and rows! and base of two! Assumes Z-curve tiling
template<int B, typename BaseMul>
class HackyStrassen2 {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    Mres c(a.rows(), b.columns());
    multiply<M0, M1, Mres>(c, a, b);
    return c;
  };

  template <typename M0, typename M1, typename Mres>
  static void multiply(Mres& c, const M0& a, const M1& b) {
    assert(a.columns() == b.rows());

    size_t n = a.rows();
    size_t p = a.columns();
    size_t m = b.columns();

    if (m <= B && n <= B && p <= B) {
      BaseMul::Template multiply<Mres, M0, M1>(c, a, b);
    } else {
      size_t new_n = n / 2;
      size_t new_p = p / 2;
      size_t new_m = m / 2;

      auto a_split = a.split();
      M0 a11 = move(get<0>(a_split));
      M0 a12 = move(get<1>(a_split));
      M0 a21 = move(get<2>(a_split));
      M0 a22 = move(get<3>(a_split));

      auto b_split = b.split();
      M1 b11 = move(get<0>(b_split));
      M1 b12 = move(get<1>(b_split));
      M1 b21 = move(get<2>(b_split));
      M1 b22 = move(get<3>(b_split));

      Mres m1(new_n, new_m);
      multiply<M0, M1, Mres>(m1, a11.unsafe_add(a22), b11.unsafe_add(b22));
      Mres m2(new_n, new_m);
      multiply<M0, M1, Mres>(m2, a21.unsafe_add(a22), b11);
      Mres m3(new_n, new_m);
      multiply<M0, M1, Mres>(m3, a11, b12.unsafe_sub(b22));
      Mres m4(new_n, new_m);
      multiply<M0, M1, Mres>(m4, a22, b21.unsafe_sub(b11));
      Mres m5(new_n, new_m);
      multiply<M0, M1, Mres>(m5, a11.unsafe_add(a12), b22);
      Mres m6(new_n, new_m);
      multiply<M0, M1, Mres>(m6, a21.unsafe_sub(a11), b11.unsafe_add(b12));
      Mres m7(new_n, new_m);
      multiply<M0, M1, Mres>(m7, a12.unsafe_sub(a22), b21.unsafe_add(b22));
      
      Mres c11 = m1.unsafe_add(m4).unsafe_sub(m5).unsafe_add(m7);
      Mres c12 = m3.unsafe_add(m5);
      Mres c21 = m2.unsafe_add(m4);
      Mres c22 = m1.unsafe_sub(m2).unsafe_add(m3).unsafe_add(m6);

      unsafe_combine<Mres>(c, c11, c12, c21, c22);
    }
  };

  template <typename Mres>
  static void unsafe_combine(Mres& c, const Mres& a11, const Mres& a12, const Mres& a21, const Mres& a22) {
    uint32_t current = 0;
    uint32_t elements = a11.rows() * a11.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a11.data.data[i];
    }

    elements = a21.rows() * a21.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a21.data.data[i];
    }

    elements = a12.rows() * a12.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a12.data.data[i];
    }

    elements = a22.rows() * a22.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a22.data.data[i];
    }
  };

  static string config() {
    return "strassen2-" + to_string(B) + "(" + BaseMul::config() + ")";
  };
};

// TODO: Assumes equal columns and rows! and base of two! Assumes Z-curve tiling
template<int B, typename BaseMul>
class ParallelHackyStrassen {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    function<Mres(const M0&, const M1&, int)> visit;
    visit = [&](const M0& a, const M1& b, int depth) -> Mres {
      assert(a.columns() == b.rows());

      size_t n = a.rows();
      size_t p = a.columns();
      size_t m = b.columns();

      if (m <= B && n <= B && p <= B) {
        return BaseMul::Template multiply<Mres, M0, M1>(a, b);
      } else {
        auto a_split = a.split();
        M0 a11 = move(get<0>(a_split));
        M0 a12 = move(get<1>(a_split));
        M0 a21 = move(get<2>(a_split));
        M0 a22 = move(get<3>(a_split));

        auto b_split = b.split();
        M1 b11 = move(get<0>(b_split));
        M1 b12 = move(get<1>(b_split));
        M1 b21 = move(get<2>(b_split));
        M1 b22 = move(get<3>(b_split));
      
        //vector<Mres> ms(7);

        Mres m1,m2,m3,m4,m5,m6,m7;

        if (depth == 0 || depth == 1) {
          thread f1 = thread([&]() { m1 = visit(a11.unsafe_add(a22), b11.unsafe_add(b22), depth + 1); });
          thread f2 = thread([&]() { m2 = visit(a21.unsafe_add(a22), b11, depth + 1); });
          thread f3 = thread([&]() { m3 = visit(a11, b12.unsafe_sub(b22), depth + 1); });
          thread f4 = thread([&]() { m4 = visit(a22, b21.unsafe_sub(b11), depth + 1); });
          thread f5 = thread([&]() { m5 = visit(a11.unsafe_add(a12), b22, depth + 1); });
          thread f6 = thread([&]() { m6 = visit(a21.unsafe_sub(a11), b11.unsafe_add(b12), depth + 1); });
          thread f7 = thread([&]() { m7 = visit(a12.unsafe_sub(a22), b21.unsafe_add(b22), depth + 1); });
          
          f1.join();
          f2.join();
          f3.join();
          f4.join();
          f5.join();
          f6.join();
          f7.join();
        } else {
          m1 = visit(a11.unsafe_add(a22), b11.unsafe_add(b22), depth + 1);
          m2 = visit(a21.unsafe_add(a22), b11, depth + 1);
          m3 = visit(a11, b12.unsafe_sub(b22), depth + 1);
          m4 = visit(a22, b21.unsafe_sub(b11), depth + 1);
          m5 = visit(a11.unsafe_add(a12), b22, depth + 1);
          m6 = visit(a21.unsafe_sub(a11), b11.unsafe_add(b12), depth + 1);
          m7 = visit(a12.unsafe_sub(a22), b21.unsafe_add(b22), depth + 1);
        }
        
        Mres c11 = m1.unsafe_add(m4).unsafe_sub(m5).unsafe_add(m7);
        Mres c12 = m3.unsafe_add(m5);
        Mres c21 = m2.unsafe_add(m4);
        Mres c22 = m1.unsafe_sub(m2).unsafe_add(m3).unsafe_add(m6);

        return ParallelHackyStrassen<B,BaseMul>::unsafe_combine<Mres>(c11, c12, c21, c22);
      };
    };

    return visit(a, b, 0);
  };

  template <typename Mres>
  static Mres unsafe_combine(const Mres& a11, const Mres& a12, const Mres& a21, const Mres& a22) {
    Mres m(a11.rows() + a21.rows(), a11.columns() + a21.columns());

    uint32_t current = 0;
    uint32_t elements = a11.rows() * a11.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a11.data.data[i];
    }

    elements = a21.rows() * a21.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a21.data.data[i];
    }

    elements = a12.rows() * a12.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a12.data.data[i];
    }

    elements = a22.rows() * a22.columns();
    for (uint32_t i = 0; i < elements; i++) {
      m.data.data[current++] = a22.data.data[i];
    }

    return m;
  };

  static string config() {
    return "par-strassen-" + to_string(B) + "(" + BaseMul::config() + ")";
  };
};

// TODO: Assumes equal columns and rows! and base of two! Assumes Z-curve tiling
template<int B, typename BaseMul>
class ParallelHackyStrassen2 {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    Mres c(a.rows(), b.columns());
    multiply<M0, M1, Mres>(c, a, b, 0);
    return c;
  };

  template <typename M0, typename M1, typename Mres>
  static void multiply(Mres& c, const M0& a, const M1& b, int depth) {
    assert(a.columns() == b.rows());

    size_t n = a.rows();
    size_t p = a.columns();
    size_t m = b.columns();

    if (m <= B && n <= B && p <= B) {
      BaseMul::Template multiply<Mres, M0, M1>(c, a, b);
    } else {
      if (depth == 0 || depth == 1) {
        size_t new_n = n / 2;
        size_t new_p = p / 2;
        size_t new_m = m / 2;

        auto a_split = a.split();
        M0 a11 = move(get<0>(a_split));
        M0 a12 = move(get<1>(a_split));
        M0 a21 = move(get<2>(a_split));
        M0 a22 = move(get<3>(a_split));

        auto b_split = b.split();
        M1 b11 = move(get<0>(b_split));
        M1 b12 = move(get<1>(b_split));
        M1 b21 = move(get<2>(b_split));
        M1 b22 = move(get<3>(b_split));

        Mres m1(new_n, new_m);
        Mres m2(new_n, new_m);
        Mres m3(new_n, new_m);
        Mres m4(new_n, new_m);
        Mres m5(new_n, new_m);
        Mres m6(new_n, new_m);
        Mres m7(new_n, new_m);

        thread f1 = thread([&]() { ParallelHackyStrassen2<B, BaseMul>::multiply(m1, a11.unsafe_add(a22), b11.unsafe_add(b22), depth + 1); });
        thread f2 = thread([&]() { ParallelHackyStrassen2<B, BaseMul>::multiply(m2, a21.unsafe_add(a22), b11, depth + 1); });
        thread f3 = thread([&]() { ParallelHackyStrassen2<B, BaseMul>::multiply(m3, a11, b12.unsafe_sub(b22), depth + 1); });
        thread f4 = thread([&]() { ParallelHackyStrassen2<B, BaseMul>::multiply(m4, a22, b21.unsafe_sub(b11), depth + 1); });
        thread f5 = thread([&]() { ParallelHackyStrassen2<B, BaseMul>::multiply(m5, a11.unsafe_add(a12), b22, depth + 1); });
        thread f6 = thread([&]() { ParallelHackyStrassen2<B, BaseMul>::multiply(m6, a21.unsafe_sub(a11), b11.unsafe_add(b12), depth + 1); });
        thread f7 = thread([&]() { ParallelHackyStrassen2<B, BaseMul>::multiply(m7, a12.unsafe_sub(a22), b21.unsafe_add(b22), depth + 1); });
          
        f1.join();
        f2.join();
        f3.join();
        f4.join();
        f5.join();
        f6.join();
        f7.join();

        Mres c11 = m1.unsafe_add(m4).unsafe_sub(m5).unsafe_add(m7);
        Mres c12 = m3.unsafe_add(m5);
        Mres c21 = m2.unsafe_add(m4);
        Mres c22 = m1.unsafe_sub(m2).unsafe_add(m3).unsafe_add(m6);

        unsafe_combine<Mres>(c, c11, c12, c21, c22);
      } else {
        HackyStrassen2<B, BaseMul>::multiply(c, a, b);
      }
    }
  };

  template <typename Mres>
  static void unsafe_combine(Mres& c, const Mres& a11, const Mres& a12, const Mres& a21, const Mres& a22) {
    uint32_t current = 0;
    uint32_t elements = a11.rows() * a11.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a11.data.data[i];
    }

    elements = a21.rows() * a21.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a21.data.data[i];
    }

    elements = a12.rows() * a12.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a12.data.data[i];
    }

    elements = a22.rows() * a22.columns();
    for (uint32_t i = 0; i < elements; i++) {
      c.data.data[current++] = a22.data.data[i];
    }
  };

  static string config() {
    return "par-strassen2-" + to_string(B) + "(" + BaseMul::config() + ")";
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
    return "recursive-" + to_string(B) + "(" + BaseCaseMultiplier::config() + ")";
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
      BaseCaseMultiplier::Template multiply<M0, M1, Mres>(result, a, b,
                                                          a_row_start, a_row_stop, a_col_start, a_col_stop,
                                                          b_row_start, b_row_stop, b_col_start, b_col_stop,
                                                          res_row_start, res_row_stop, res_col_start, res_col_stop);
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

/*
 * CreateThreadDepth: Depth in recursion where recursive calls are spawned
 *   in new threads. Hence 2^{CreateThreadLevel} threads will be created!
 */
template<int B, typename BaseCaseMultiplier, int CreateThreadDepth>
class ParallelRecursive {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());
    
    Mres c(a.rows(), b.columns(), 0);
    Multiplier<M0, M1, Mres>(c, a, b)();
    
    return c;
  }
  
  static string config() {
    return "par-recursive-" + to_string(B) + + "(" + BaseCaseMultiplier::config() + ")";
  };
  
  
private:
  template <typename M0, typename M1, typename Mres>
  class Multiplier {
  private:
    Mres& result;
    const M0& a;
    const M1& b;
    
    void visit(uint32_t a_row_start, uint32_t a_row_stop,
               uint32_t a_col_start, uint32_t a_col_stop,
               uint32_t b_row_start, uint32_t b_row_stop,
               uint32_t b_col_start, uint32_t b_col_stop,
               uint32_t res_row_start, uint32_t res_row_stop,
               uint32_t res_col_start, uint32_t res_col_stop,
               unsigned short thread_depth)
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
        auto run = [a_row_start, a_col_start, a_col_stop,
                    b_row_start, b_row_stop, b_col_start, b_col_stop,
                    res_row_start, res_col_start, res_col_stop,
                    thread_depth, mid, this] () {
          this->visit(a_row_start, a_row_start + mid, a_col_start, a_col_stop,
                      b_row_start, b_row_stop, b_col_start, b_col_stop,
                      res_row_start, res_row_start + mid, res_col_start, res_col_stop,
                      thread_depth + 1);
        };
        
        thread th;
        if (thread_depth < CreateThreadDepth) {
          th = thread(run);
        } else {
          run();
        }
        
        visit(a_row_start + mid, a_row_stop, a_col_start, a_col_stop,
              b_row_start, b_row_stop, b_col_start, b_col_stop,
              res_row_start + mid, res_row_stop, res_col_start, res_col_stop,
              thread_depth + 1);
        
        if (th.joinable())
          th.join();
      } else if (n > max(m, p)) {
        // Split both
        const uint32_t mid = n / 2;
        visit(a_row_start, a_row_stop, a_col_start, a_col_start + mid,
              b_row_start, b_row_start + mid, b_col_start, b_col_stop,
              res_row_start, res_row_stop, res_col_start, res_col_stop,
              thread_depth);
        visit(a_row_start, a_row_stop, a_col_start + mid, a_col_stop,
              b_row_start + mid, b_row_stop, b_col_start, b_col_stop,
              res_row_start, res_row_stop, res_col_start, res_col_stop,
              thread_depth);
      } else {
        assert(p >= max(m, n));
        
        // Split b
        const uint32_t mid = p / 2;
        auto run = [a_row_start, a_row_stop, a_col_start, a_col_stop,
                    b_row_start, b_row_stop, b_col_start,
                    res_row_start, res_row_stop, res_col_start,
                    thread_depth, mid, this] () {
          this->visit(a_row_start, a_row_stop, a_col_start, a_col_stop,
                      b_row_start, b_row_stop, b_col_start, b_col_start + mid,
                      res_row_start, res_row_stop, res_col_start, res_col_start + mid,
                      thread_depth + 1);
        };
        thread th;
        if (thread_depth < CreateThreadDepth) {
          th = thread(run);
        } else {
          run();
        }
        
        visit(a_row_start, a_row_stop, a_col_start, a_col_stop,
              b_row_start, b_row_stop, b_col_start + mid, b_col_stop,
              res_row_start, res_row_stop, res_col_start + mid, res_col_stop,
              thread_depth + 1);
        
        if (th.joinable())
          th.join();
      }
    }
    
  public:
    Multiplier(Mres& result, const M0& a, const M1& b)
      : result(result), a(a), b(b)
    { }
    
    // Assumption c is 0 initialized
    void operator() ()
    {
      visit(0, a.rows(), 0, a.columns(),
            0, b.rows(), 0, b.columns(),
            0, result.rows(), 0, result.columns(),
            0);
    }
  };
};

template <int B, typename BaseMul>
class TiledIterative {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());
    assert(a.rows() % B == 0);
    assert(a.columns() % B == 0);
    assert(b.columns() % B == 0);
    
    const uint32_t tile_n = a.rows() / B;
    const uint32_t tile_m = b.columns() / B;
    const uint32_t tile_p = a.columns() / B;
    
    Mres c(a.rows(), b.columns(), 0);
    
    for (uint32_t i = 0; i < tile_n; i++) {
      for (uint32_t j = 0; j < tile_m; j++) {
        const uint32_t c_row_start = i * B;
        const uint32_t c_col_start = j * B;
        const uint32_t c_row_stop = c_row_start + B;
        const uint32_t c_col_stop = c_col_start + B;
        
        for (uint32_t k = 0; k < tile_p; k++) {
          
          const uint32_t a_row_start = i * B;
          const uint32_t a_col_start = k * B;
          const uint32_t a_row_stop = a_row_start + B;
          const uint32_t a_col_stop = a_col_start + B;
          
          const uint32_t b_row_start = k * B;
          const uint32_t b_col_start = j * B;
          const uint32_t b_row_stop = b_row_start + B;
          const uint32_t b_col_stop = b_col_start + B;
          
          BaseMul::Template multiply<M0, M1, Mres>(c, a, b,
						   a_row_start, a_row_stop,
						   a_col_start, a_col_stop,
						   b_row_start, b_row_stop,
						   b_col_start, b_col_stop,
						   c_row_start, c_row_stop,
						   c_col_start, c_col_stop);
        }
      }
    }
    
    // Move semantics
    return c;
  };
  
  static string config() {
    return "tiled-iterative(" + BaseMul::config() + ")";
  };
};

template <int B, int T, typename BaseMul>
class ParallelTiledIterative {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());
    assert(a.rows() % B == 0);
    assert(a.columns() % B == 0);
    assert(b.columns() % B == 0);
    
    const uint32_t tile_n = a.rows() / B;
    const uint32_t tile_m = b.columns() / B;
    const uint32_t tile_p = a.columns() / B;
    
    Mres c(a.rows(), b.columns(), 0);
    
    vector<thread> threads;
    
    assert(tile_n % T == 0);
    
    size_t slice = tile_n / T;
    
    for (int t = 0; t < T; t++) {
      uint32_t from = t * slice;
      uint32_t to = t == T - 1 ? tile_n : (t + 1) * slice;
      
      auto run = [&a, &b, &c, from, to, tile_n, tile_p, tile_m]() {
        for (uint32_t i = from; i < to; i++) {
          for (uint32_t j = 0; j < tile_m; j++) {
            const uint32_t c_row_start = i * B;
            const uint32_t c_col_start = j * B;
            const uint32_t c_row_stop = c_row_start + B;
            const uint32_t c_col_stop = c_col_start + B;
            
            for (uint32_t k = 0; k < tile_p; k++) {
              
              const uint32_t a_row_start = i * B;
              const uint32_t a_col_start = k * B;
              const uint32_t a_row_stop = a_row_start + B;
              const uint32_t a_col_stop = a_col_start + B;
              
              const uint32_t b_row_start = k * B;
              const uint32_t b_col_start = j * B;
              const uint32_t b_row_stop = b_row_start + B;
              const uint32_t b_col_stop = b_col_start + B;
              
              BaseMul::Template multiply<M0, M1, Mres>(c, a, b,
						       a_row_start, a_row_stop,
						       a_col_start, a_col_stop,
						       b_row_start, b_row_stop,
						       b_col_start, b_col_stop,
						       c_row_start, c_row_stop,
						       c_col_start, c_col_stop);
            }
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
    return "par-tiled-iterative(" + BaseMul::config() + ")";
  };
};
