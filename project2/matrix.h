#pragma once

#include <cstddef>
#include <stdexcept>
#include <memory>
#include <cassert>
#include <vector>
#include <string>

using namespace std;

template <class Layout, typename MatrixMul>
class Matrix {
  Layout data;

  typedef Matrix<Layout, MatrixMul> type;

public:
  typedef typename Layout::Element Element;

  explicit Matrix(size_t n, size_t m) : data(Layout(n, m)) { };
  Matrix(Matrix&& other) : data(move(other.data)) { cout << "Matrix. Moving resource." << endl; }
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
};
