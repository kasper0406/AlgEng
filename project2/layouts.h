#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>
#include <iostream>
#include <stdlib.h>

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

  inline void overwrite_entries(Element e) {
    throw runtime_error("Not implemented!");
  }
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

  explicit DataLayout(size_t n, size_t m) : BaseLayout<Element>(n, m), data(nullptr) {
#ifndef _WINDOWS
    int res = posix_memalign((void**)&data, CACHE_LINE_SIZE, n * m * sizeof(Element));
    if (res != 0)
      throw runtime_error("Could not allocate memory!");
#else
    data = new Element[n * m];
#endif
  };

  inline void overwrite_entries(Element e) {
    const uint32_t size = this->n * this->m;
    for (uint32_t i = 0; i < size; i++)
      data[i] = e;
  }

  ~DataLayout() {
    if (data != nullptr) {
#ifndef WINDOWS
      delete[] data;
#elif
      free(data);
#endif
    }
  };
  
private:
  static const size_t CACHE_LINE_SIZE = 64;
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

template <typename Element>
class ZCurve : public DataLayout<Element> {
public:
  // Constructors and move semantics
  explicit ZCurve(size_t n, size_t m) : DataLayout<Element>(n, m) { };
  ZCurve(ZCurve&& other) : DataLayout<Element>(move(other)) { };
  ZCurve& operator=(ZCurve&& other)
  {
    DataLayout<Element>::operator= (move(other));
    return *this;
  }

  inline size_t interleave_bits(size_t row, size_t column) const {
    static const size_t B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const size_t S[] = {1, 2, 4, 8};

    size_t x = row;
    size_t y = column;
    size_t z;

    x = (x | (x << S[3])) & B[3];
    x = (x | (x << S[2])) & B[2];
    x = (x | (x << S[1])) & B[1];
    x = (x | (x << S[0])) & B[0];

    y = (y | (y << S[3])) & B[3];
    y = (y | (y << S[2])) & B[2];
    y = (y | (y << S[1])) & B[1];
    y = (y | (y << S[0])) & B[0];

    z = x | (y << 1);

    return z;
  };

  inline Element operator()(size_t row, size_t column) const {
    assert(row < this->n && column < this->m);
    return this->data[interleave_bits(row, column)];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return this->data[interleave_bits(row, column)];
  };

  static string config() {
    return "z-curve";
  };
};

template <uint32_t W, uint32_t H, typename Element>
class RowTiled : public DataLayout<Element> {
public:
  // Constructors and move semantics
  explicit RowTiled(size_t n, size_t m) : DataLayout<Element>(n, m) { };
  RowTiled(RowTiled&& other) : DataLayout<Element>(move(other)) { };
  RowTiled& operator=(RowTiled&& other)
  {
    DataLayout<Element>::operator= (move(other));
    return *this;
  }
  
  inline size_t calculate_index(size_t row, size_t column) const {
    const uint32_t C = column / W;
    const uint32_t R = row / H;
    return C * H * W + R * H * this->n + (row % H) * W + (column % W);
  }
  
  inline Element operator()(size_t row, size_t column) const {
    assert(row < this->n && column < this->m);
    return this->data[calculate_index(row, column)];
  };
  
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return this->data[calculate_index(row, column)];
  };
  
  static string config() {
    return "row-tiled" + to_string(W) + "x" + to_string(H);
  };
};

template <uint32_t W, uint32_t H, typename Element>
class ColumnTiled : public DataLayout<Element> {
public:
  // Constructors and move semantics
  explicit ColumnTiled(size_t n, size_t m) : DataLayout<Element>(n, m) { };
  ColumnTiled(ColumnTiled&& other) : DataLayout<Element>(move(other)) { };
  ColumnTiled& operator=(ColumnTiled&& other)
  {
    DataLayout<Element>::operator= (move(other));
    return *this;
  }
  
  inline size_t calculate_index(size_t row, size_t column) const {
    const uint32_t C = column / W;
    const uint32_t R = row / H;
    return C * H * W + R * H * this->n + (row % H) + (column % W) * H;
  }
  
  inline Element operator()(size_t row, size_t column) const {
    assert(row < this->n && column < this->m);
    return this->data[calculate_index(row, column)];
  };
  
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return this->data[calculate_index(row, column)];
  };
  
  static string config() {
    return "column-tiled-" + to_string(W) + "x" + to_string(H);
  };
};
