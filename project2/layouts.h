#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>

using namespace std;

template <typename Element>
class BaseLayout {
public:
  size_t n, m;

  typedef Element Element;

  BaseLayout(size_t n, size_t m) : n(n), m(m) {
  };

  inline size_t rows() const {
    return n;
  };

  inline size_t columns() const {
    return m;
  };
};

template <typename Element>
class DataLayout : public BaseLayout<Element> {
public:
  Element* data;

  DataLayout(size_t n, size_t m) : BaseLayout(n, m), data(new Element[n * m]) {
  };

  ~DataLayout() {
    delete[] data;
  };
};

template <typename Element>
class RowBased : public DataLayout<Element> {
public:
  RowBased(size_t n, size_t m) : DataLayout(n, m) {
  };

  inline Element operator()(size_t row, size_t column) const {
    assert(row < n && column < m);
    return data[row * m + column];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < n && column < m);
    return data[row * m + column];
  };

  static string config() {
    return "row-based";
  };
};

template <typename Element>
class ColumnBased : public DataLayout<Element> {
public:
  ColumnBased(size_t n, size_t m) : DataLayout(n, m) {
  };

  inline Element operator()(size_t row, size_t column) const {
    assert(row < n && column < m);
    return data[column * n + row];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < n && column < m);
    return data[column * n + row];
  };

  static string config() {
    return "column-based";
  };
};
