#pragma once

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <cassert>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

template <class L, typename MatrixMul>
class Matrix {
public:
  typedef L Layout;
  typedef typename Layout::Element Element;

  explicit Matrix(size_t n, size_t m) : data(Layout(n, m)) { };

  explicit Matrix(size_t n, size_t m, typename Layout::Element init) : data(Layout(n, m))
  {
    data.overwrite_entries(init);
  }

  Matrix(Matrix&& other) : data(move(other.data)) { }
  Matrix(const Matrix& other) : data(other.data) { throw logic_error("Do not copy matrices!"); }
  Matrix& operator=(Matrix&& other)
  {
    cout << "Matrix Assignment" << endl;
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

  template <typename M>
  bool operator==(const M& other) const {
    if (this->rows() != other.rows() || this->rows() != other.columns()) {
      return false;
    }

    for (uint32_t i = 0; i < this->rows(); i++) {
      for (uint32_t j = 0; j < this->columns(); j++) {
        if (abs(data(i, j) - other(i, j)) > 0.0000000000001)
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

  inline size_t rows() const {
    return data.rows();
  };

  inline size_t columns() const {
    return data.columns();
  };

  static string config() {
    return Layout::config() + " " + MatrixMul::config();
  };
    
private:
  typedef Matrix<Layout, MatrixMul> type;    
  Layout data;
};
