#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <string>
#include <iostream>
#include <unordered_map>
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
      delete[] data;

      data = other.data;
      dont_free = other.dont_free;
      BaseLayout<Element>::operator= (move(other));

      other.data = nullptr;
    }
    return *this;
  }

  explicit DataLayout(size_t n, size_t m, Element* data0) : BaseLayout<Element>(n, m), data(data0), dont_free(true) { };

  explicit DataLayout(size_t n, size_t m) : BaseLayout<Element>(n, m), data(nullptr), dont_free(false) {
#ifndef _WINDOWS
    int res = posix_memalign((void**)&data, CACHE_LINE_SIZE, n * m * sizeof(Element));
    if (res != 0)
      throw runtime_error("Could not allocate memory!");
#else
    data = new Element[n * m];
#endif
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

  ~DataLayout() {
    if (data != nullptr && !dont_free) {
#ifndef WINDOWS
      free(data);
#else
      delete[] data;
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
  ZCurveTiled(ZCurveTiled&& other) : DataLayout<Element>(move(other)) { };
  ZCurveTiled& operator=(ZCurveTiled&& other)
  {
    DataLayout<Element>::operator= (move(other));
    return *this;
  }

  static const uint32_t WIDTH = B;
  static const uint32_t HEIGHT = B;

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
    return data[calculate_index(row, column)];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return data[calculate_index(row, column)];
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

template <>
struct hash<pair<uint32_t, uint32_t> > {
public:
  size_t operator()(pair<uint32_t, uint32_t> x) const throw() {
    return x.first * 3 + x.second * 7;
  }
};

typedef unordered_map<pair<uint32_t, uint32_t>, uint32_t*> OffsetMap;

template <typename Element>
class CachingZCurve : public DataLayout<Element> {
public:
  // Constructors and move semantics
  explicit CachingZCurve(size_t n, size_t m) : DataLayout<Element>(n, m) {
    OffsetMap::const_iterator got = CachingZCurve<Element>::offset_vectors.find(make_pair(n, m));
    if (got == offset_vectors.end()) {
      uint32_t* new_offsets = make_offsets(n, m);
      offset_vectors.insert(OffsetMap::value_type(make_pair(n, m), new_offsets));
      offsets = new_offsets;
    } else {
      offsets = got->second;
    }
  };
  CachingZCurve(CachingZCurve&& other) : DataLayout<Element>(move(other)) {
    offsets = other.offsets;
    other.offsets = nullptr;
  };
  CachingZCurve& operator=(CachingZCurve&& other)
  {
    if (this != &other) {
      DataLayout<Element>::operator= (move(other));
      offsets = other.offsets;
      other.offsets = nullptr;
    }
    return *this;
  };

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
    return this->data[offsets[row * this->n + column]];
  };
    
  inline Element& operator()(size_t row, size_t column) {
    assert(row < this->n && column < this->m);
    return this->data[offsets[row * this->n + column]];
  };

  static string config() {
    return "caching-z-curve";
  };
private:
  static uint32_t* make_offsets(size_t rows, size_t columns) {
    uint32_t* offsets = new uint32_t[rows * columns];
    
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++)
        offsets[i * rows + j] = interleave_bits(i, j);
    }

    return offsets;
  };
  
  const uint32_t* offsets;
  static OffsetMap offset_vectors;
};
// TODO: Bliver ikke free'et
template<typename Element> OffsetMap CachingZCurve<Element>::offset_vectors = OffsetMap();

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
