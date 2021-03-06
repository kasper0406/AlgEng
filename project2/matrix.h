#pragma once

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

// AVX intrinsics
#include <immintrin.h>

using namespace std;

template <class L, typename MatrixMul, bool SIMD_ = false>
class Matrix {
public:
  typedef L Layout;
  typedef typename Layout::Element Element;
  typedef Matrix<L, MatrixMul, SIMD_> SelfType;

  const static bool SIMD = SIMD_;
  
  explicit Matrix(size_t n, size_t m) : data(Layout(n, m)) { };

  explicit Matrix(Layout data1) : data(move(data1)) { };

  explicit Matrix() : data(Layout(0,0,nullptr)) { };

  explicit Matrix(size_t n, size_t m, typename Layout::Element init) : data(Layout(n, m))
  {
    data.overwrite_entries(init);
  }

  Matrix(Matrix&& other) : data(move(other.data)) {
  }

  Matrix(const Matrix& other) : data(other.data) { throw logic_error("Do not copy matrices!"); }
  Matrix& operator=(Matrix& other) { throw logic_error("Do not copy matrices!"); };
  Matrix& operator=(Matrix&& other)
  {
    if (this != &other) {
      data = move(other.data);
    }
    return *this;
  }

  Matrix(size_t n, size_t m, vector<vector<Element>> elements) : Matrix(n, m) {
    assert(elements.size() == n);

    for (uint32_t i = 0; i < elements.size(); i++) {
      assert(elements[i].size() == m);

      for (uint32_t j = 0; j < elements[i].size(); j++) {
        data(i, j) = elements[i][j];
      }
    }
  };

  Matrix(const Matrix& other, uint32_t from_n, uint32_t to_n, uint32_t from_m, uint32_t to_m) : data(Layout(to_n - from_n, to_m - from_m)) {
    for (uint32_t i = from_n; i < to_n; i++) {
      for (uint32_t j = from_m; j < to_m; j++) {
        data(i - from_n, j - from_m) = other(i, j);
      }
    }
  };

  inline Element operator()(size_t row, size_t column) const {
    return data(row, column);
  };
    
  inline Element& operator()(size_t row, size_t column) {
    return data(row, column);
  };
    
  inline Element at(size_t index) const {
    return data.at(index);
  };
    
  inline Element& at(size_t index) {
    return data.at(index);
  };
  
  inline Element* addr(size_t index) const {
    return data.addr(index);
  }

  template <typename M>
  M convert() {
    M other(this->rows(), this->columns());

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        other(i, j) = data(i, j);
      }
    }

    return other;
  };

  tuple<SelfType, SelfType, SelfType, SelfType> split() const {
    return data.Template split<SelfType>();
  };

  template <typename M>
  bool operator==(const M& other) const {
    if (this->rows() != other.rows() || this->rows() != other.columns()) {
      return false;
    }

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        if (abs(data(i, j) - other(i, j)) > 0.000000000001)
          return false;
      }
    }

    return true;
  };

  template <typename M>
  bool operator!=(const M& other) const {
    return !this->operator==(other);
  };

  template <typename M1, typename Mres>
  Mres operator*(const M1& other) const {
    return MatrixMul::template multiply<type, M1, Mres>(*this, other);
  };

  SelfType unsafe_add(const SelfType& other) const {
    const size_t N = this->rows() * this->columns();
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());
    SelfType c(this->rows(), this->columns());
    
    if (!SIMD) {
      
      for (uint32_t i = 0; i < N; i++) {
        c.data.data[i] = this->data.data[i] + other.data.data[i];
      }
    } else {
      static_assert(is_same<typename SelfType::Element, double>::value,
                    "Element type must be double.");
      assert(N % 4 == 0);
      
      for (size_t i = 0; i < N; i += 4) {
        __m256d a = _mm256_load_pd(this->addr(i));
        __m256d b = _mm256_load_pd(other.addr(i));
        
        __m256d res = _mm256_add_pd(a, b);
        
        _mm256_store_pd(c.addr(i), res);
      }
    }

    // Move semantics
    return c;
  };
  
  SelfType unsafe_sub(const SelfType& other) const {
    const size_t N = this->rows() * this->columns();
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());
    SelfType c(this->rows(), this->columns());
    
    if (!SIMD) {
      
      for (uint32_t i = 0; i < N; i++) {
        c.data.data[i] = this->data.data[i] - other.data.data[i];
      }
    } else {
      static_assert(is_same<typename SelfType::Element, double>::value,
                    "Element type must be double.");
      assert(N % 4 == 0);
      
      for (size_t i = 0; i < N; i += 4) {
        __m256d a = _mm256_load_pd(this->addr(i));
        __m256d b = _mm256_load_pd(other.addr(i));
        
        __m256d res = _mm256_sub_pd(a, b);
        
        _mm256_store_pd(c.addr(i), res);
      }
    }

    // Move semantics
    return c;
  };

  inline size_t rows() const {
    return data.rows();
  };

  inline size_t columns() const {
    return data.columns();
  };

  static string config() {
    return Layout::config() + " " + MatrixMul::config() + (SIMD == true ? "_SIMD" : "");
  };

  string to_string() const {
    stringstream ss;
    ss.precision(3);

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        ss << data(i, j) << "\t";
      }
      ss << endl;
    }

    return ss.str();
  };
  
  Layout data;
private:
  typedef Matrix<Layout, MatrixMul, SIMD_> type;
};
