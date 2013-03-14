#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>
#include <iostream>

using namespace std;

template <typename Element>
class BaseLayout {
public:
  size_t n, m;

  typedef Element Element;

  explicit BaseLayout(size_t n, size_t m) : n(n), m(m) {
  };

  BaseLayout(BaseLayout&& other)
  {
    n = other.n;
    m = other.m;
  }

  BaseLayout& operator=(BaseLayout&& other)
  {
    if (this != &other)
    {
      n = other.n;
      m = other.m;
    }
    return *this;
  }

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

  DataLayout(DataLayout&& other) : BaseLayout(move(other))
  {
    data = other.data;
    other.data = nullptr;
  }

  DataLayout& operator=(DataLayout&& other)
  {
    if (this != &other)
    {
      delete[] data;

      data = other.data;
      BaseLayout::operator= (move(other));

      other.data = nullptr;
    }
    return *this;
  }

  explicit DataLayout(size_t n, size_t m) : BaseLayout(n, m), data(new Element[n * m]) {
  };

  ~DataLayout() {
    if (data != nullptr)
      delete[] data;
  };
};

template <typename Element>
class RowBased : public DataLayout<Element> {
public:
  // Constructors and move semantics
  explicit RowBased(size_t n, size_t m) : DataLayout(n, m) { };
  RowBased(RowBased&& other) : DataLayout(move(other)) { };
  RowBased& operator=(RowBased&& other)
  {
    DataLayout::operator= (move(other));
    return *this;
  }

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
  // Constructors and move semantics
  explicit ColumnBased(size_t n, size_t m) : DataLayout(n, m) { };
  ColumnBased(ColumnBased&& other) : DataLayout(move(other)) { };
  ColumnBased& operator=(ColumnBased&& other)
  {
    DataLayout::operator= (move(other));
    return *this;
  }

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