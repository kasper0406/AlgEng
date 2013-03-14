#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>
#include <iostream>

using namespace std;

template <typename E>
class BaseLayout {
public:
  size_t n, m;

  typedef E Element;

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

  DataLayout(DataLayout&& other) : BaseLayout<Element>(move(other))
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
      BaseLayout<Element>::operator= (move(other));

      other.data = nullptr;
    }
    return *this;
  }

  explicit DataLayout(size_t n, size_t m) : BaseLayout<Element>(n, m), data(new Element[n * m]) {
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
  explicit RowBased(size_t n, size_t m) : DataLayout<Element>(n, m) { };
  RowBased(RowBased&& other) : DataLayout<Element>(move(other)) { };
  RowBased& operator=(RowBased&& other)
  {
    DataLayout<Element>::operator= (move(other));
    return *this;
  }

  inline Element operator()(size_t row, size_t column) const {
    assert(row < this->n && column < this->m);
    return this->data[row * this->m + column];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return this->data[row * this->m + column];
  };

  static string config() {
    return "row-based";
  };
};

template <typename Element>
class ColumnBased : public DataLayout<Element> {
public:
  // Constructors and move semantics
  explicit ColumnBased(size_t n, size_t m) : DataLayout<Element>(n, m) { };
  ColumnBased(ColumnBased&& other) : DataLayout<Element>(move(other)) { };
  ColumnBased& operator=(ColumnBased&& other)
  {
    DataLayout<Element>::operator= (move(other));
    return *this;
  }

  inline Element operator()(size_t row, size_t column) const {
    assert(row < this->n && column < this->m);
    return this->data[column * this->n + row];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return this->data[column * this->n + row];
  };

  static string config() {
    return "column-based";
  };
};
