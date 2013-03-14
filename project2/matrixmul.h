#pragma once

#include <cstddef>
#include <cassert>
#include <memory>
#include <string>
#include "matrix.h"
#include <iostream>

using namespace std;

class Naive {
public:
  template <typename M0, typename M1, typename Mres>
  static Mres multiply(const M0& a, const M1& b) {
    assert(a.columns() == b.rows());

    Mres c(a.rows(), b.columns());

    for (uint32_t i = 0; i < a.rows(); i++) {
      for (uint32_t j = 0; j < b.columns(); j++) {
        typename Mres::Element e(0);

        for (uint32_t k = 0; k < a.columns(); k++) {
          e += a(i, k) * b(k, j);
        }

        c(i, j) = e;
      }
    }
    
    // Move semantics
    return c;
  };

  static string config() {
    return "naive";
  };
};
