#include <iostream>

#include "matrix.h"
#include "layouts.h"
#include "matrixmul.h"
#include "test.h"

typedef Matrix<RowBased<double>, Naive> RN;
typedef Matrix<ZCurve<double>, Naive> RZ;
typedef Matrix<ColumnBased<double>, Naive> CN;

using namespace std;

int main(int argc, char *argv[]) {
  cout.precision(8);
  
  // Test 1
  //test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  //test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);

  // Test 2
  test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RZ,RZ,RZ>(cout, 1, 1024, 1024 * 1024 * 1024);

  return 0;
}
