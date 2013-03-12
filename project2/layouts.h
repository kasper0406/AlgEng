#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>

using namespace std;

template <typename Element>
class RowBased {
  Element* data;
  size_t n, m;

public:
  typedef Element Element;

  RowBased(size_t n, size_t m) : n(n), m(m), data(new Element[n * m]) {
  };

  inline Element operator()(size_t row, size_t column) const {
    assert(row < n && column < m);
    return data[row * m + column];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < n && column < m);
    return data[row * m + column];
  };

  inline size_t rows() const {
    return n;
  };

  inline size_t columns() const {
    return m;
  };

  static string config() {
    return "row-based";
  };

  ~RowBased() {
    delete[] data;
  };
};