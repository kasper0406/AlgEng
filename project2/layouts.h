#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdlib.h>

#include "stackallocator.h"

using namespace std;

#ifdef _STACKALLOC
StackAllocator stackalloc(_STACKALLOC);
#endif

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
  bool dont_free;

  DataLayout(DataLayout&& other) : BaseLayout<Element>(move(other))
  {
    data = other.data;
    dont_free = other.dont_free;
    other.data = nullptr;
  }

  DataLayout& operator=(DataLayout&& other)
  {
    if (this != &other)
    {
      if (data != nullptr) {
#ifdef _STACKALLOC
        stackalloc.free(data);
#else
#ifndef _WINDOWS
        free(data);
#else
        delete[] data;
#endif
#endif
      }

      BaseLayout<Element>::operator= (move(other));
      data = other.data;
      dont_free = other.dont_free;

      other.data = nullptr;
    }
    return *this;
  };

  explicit DataLayout(size_t n, size_t m, Element* data0) : BaseLayout<Element>(n, m), data(data0), dont_free(true) { };

  explicit DataLayout(size_t n, size_t m) : BaseLayout<Element>(n, m), data(nullptr), dont_free(false) {
    if (stack_allocate) {
      data = stackalloc.alloc<Element>(n * m);
    } else {
#ifndef _WINDOWS
      int res = posix_memalign((void**)&data, CACHE_LINE_SIZE, n * m * sizeof(Element));
      if (res != 0)
        throw runtime_error("Could not allocate memory!");
#else
      data = new Element[n * m];
#endif
    }
  };

  template <typename M>
  tuple<M, M, M, M> split() {
    throw logic_error("Not valid for this layout!");
  };

  inline void overwrite_entries(Element e) {
    const uint32_t size = this->n * this->m;
    for (uint32_t i = 0; i < size; i++)
      data[i] = e;
  }
  
  inline Element at(size_t index) const {
    assert(index < this->n * this->m);
    return this->data[index];
  };
  
  inline Element& at(size_t index) {
    assert(index < this->n * this->m);
    return this->data[index];
  };
  
  inline Element* addr(size_t index) const {
    return this->data + index;
  }

  ~DataLayout() {
    if (data != nullptr && !dont_free) {
      if (stack_allocate) {
        stackalloc.free(data);
      } else {
#ifndef _WINDOWS
        free(data);
#else
        delete[] data;
#endif
      }
    }
  };
  
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

  static inline size_t interleave_bits(size_t row, size_t column) {
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

template <typename Element, int B, bool row_tiled>
class ZCurveTiled : public DataLayout<Element> {
public:
  // Constructors and move semantics
  explicit ZCurveTiled(size_t n, size_t m) : DataLayout<Element>(n, m) { };
  ZCurveTiled(ZCurveTiled&& other) : DataLayout<Element>(move(other)) {
  };
  ZCurveTiled& operator=(ZCurveTiled&& other)
  {
    DataLayout<Element>::operator= (move(other));
    return *this;
  }

  static const uint32_t WIDTH = B;
  static const uint32_t HEIGHT = B;

  static inline size_t interleave_bits(size_t row, size_t column) {
    static const size_t K[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
    static const size_t S[] = {1, 2, 4, 8};

    size_t x = row;
    size_t y = column;
    size_t z;

    x = (x | (x << S[3])) & K[3];
    x = (x | (x << S[2])) & K[2];
    x = (x | (x << S[1])) & K[1];
    x = (x | (x << S[0])) & K[0];

    y = (y | (y << S[3])) & K[3];
    y = (y | (y << S[2])) & K[2];
    y = (y | (y << S[1])) & K[1];
    y = (y | (y << S[0])) & K[0];

    z = x | (y << 1);

    return z;
  };

  inline uint32_t calculate_index(size_t row, size_t column) const {
    uint32_t tile_x = row / B;
    uint32_t tile_y = column / B;
    uint32_t tile = interleave_bits(tile_x, tile_y);
    uint32_t tile_column = column % B;
    uint32_t tile_row = row % B;
    if (row_tiled) {
      return tile * B * B + tile_row * B + tile_column;
    } else {
      return tile * B * B + tile_row + B * tile_column;
    }
  };

  inline Element operator()(size_t row, size_t column) const {
    assert(row < this->n && column < this->m);
    return this->data[calculate_index(row, column)];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return this->data[calculate_index(row, column)];
  };

  ZCurveTiled(size_t n, size_t m, Element* data0) : DataLayout<Element>(n, m, data0) { };

  template <typename M>
  tuple<M, M, M, M> split() const {
    size_t n = this->rows();
    size_t m = this->columns();

    size_t new_n = n / 2;
    size_t new_m = m / 2;

    M a11(ZCurveTiled(new_n, new_m, this->data + calculate_index(0, 0)));
    M a12(ZCurveTiled(new_n, new_m, this->data + calculate_index(0, new_m)));
    M a21(ZCurveTiled(new_n, new_m, this->data + calculate_index(new_n, 0)));
    M a22(ZCurveTiled(new_n, new_m, this->data + calculate_index(new_n, new_m)));

    return make_tuple(move(a11), move(a12), move(a21), move(a22));
  };

  static string config() {
    return "z-curve-tiled";
  };
};

template <uint32_t W, uint32_t H, typename Element>
class RowTiled : public DataLayout<Element> {
public:
  static const uint32_t WIDTH = W;
  static const uint32_t HEIGHT = H;
  
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
  static const uint32_t WIDTH = W;
  static const uint32_t HEIGHT = H;
  
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
