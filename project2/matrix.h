#pragma once

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

template <class L, typename MatrixMul>
class Matrix {
public:
  typedef L Layout;
  typedef typename Layout::Element Element;
  typedef typename Matrix<L, MatrixMul> SelfType;

  explicit Matrix(size_t n, size_t m) : data(Layout(n, m)) { };

  explicit Matrix(Layout data1) : data(data1) { };

  explicit Matrix(size_t n, size_t m, typename Layout::Element init) : data(Layout(n, m))
  {
    data.overwrite_entries(init);
  }

  Matrix(Matrix&& other) : data(move(other.data)) { }
  Matrix(const Matrix& other) : data(other.data) { throw logic_error("Do not copy matrices!"); }
  Matrix& operator=(Matrix&& other)
  {
    if (this != &other) {
      data = other.data;
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

  Matrix(const Matrix& a11, const Matrix& a12, const Matrix& a21, const Matrix& a22) : data(Layout(a11.rows() + a21.rows(), a11.columns() + a21.columns())) {
    assert(a11.rows() == a12.rows());
    assert(a21.rows() == a22.rows());
    assert(a11.columns() == a21.columns());
    assert(a12.columns() == a22.columns());


    // A11 and A12
    for (uint32_t i = 0; i < a11.rows(); i++) {
      for (uint32_t j = 0; j < a11.columns(); j++) {
        data(i, j) = a11(i, j);
      }

      for (uint32_t j = 0; j < a12.columns(); j++) {
        data(i, j + a11.columns()) = a12(i, j);
      }
    }

    // A21 and A22
    for (uint32_t i = 0; i < a21.rows(); i++) {
      for (uint32_t j = 0; j < a21.columns(); j++) {
        data(i + a11.rows(), j) = a21(i, j);
      }

      for (uint32_t j = 0; j < a22.columns(); j++) {
        data(i + a11.rows(), j + a21.columns()) = a22(i, j);
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
    return data.split<SelfType>();
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

  template <typename M1, typename Mres>
  Mres operator+(const M1& other) const {
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());

    Mres c(this->rows(), this->columns());

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        c(i, j) = this->operator()(i, j) + other(i, j);
      }
    }
    
    // Move semantics
    return c;
  };

  template <>
  SelfType operator+(const SelfType& other) const {
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());

    SelfType c(this->rows(), this->columns());

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        c.data.data[i * this->rows() + j] = this->data.data[i * this->rows() + j] + other.data.data[i * this->rows() + j];
      }
    }
    
    // Move semantics
    return c;
  };

  template <typename M1, typename Mres>
  Mres operator-(const M1& other) const {
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());

    Mres c(this->rows(), this->columns());

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        c(i, j) = this->operator()(i, j) - other(i, j);
      }
    }
    
    // Move semantics
    return c;
  };

  SelfType unsafe_add(const SelfType& other) const {
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());

    SelfType c(this->rows(), this->columns());

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        c.data.data[i * this->rows() + j] = this->data.data[i * this->rows() + j] + other.data.data[i * this->rows() + j];
      }
    }
    
    // Move semantics
    return c;
  };

  SelfType unsafe_sub(const SelfType& other) const {
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());

    SelfType c(this->rows(), this->columns());

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        c.data.data[i * this->rows() + j] = this->data.data[i * this->rows() + j] - other.data.data[i * this->rows() + j];
      }
    }
    
    // Move semantics
    return c;
  };

  template <>
  SelfType operator-(const SelfType& other) const {
    assert(this->columns() == other.columns());
    assert(this->rows() == other.rows());

    SelfType c(this->rows(), this->columns());

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        c.data.data[i * this->rows() + j] = this->data.data[i * this->rows() + j] - other.data.data[i * this->rows() + j];
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
    return Layout::config() + " " + MatrixMul::config();
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
  typedef Matrix<Layout, MatrixMul> type;    
};
