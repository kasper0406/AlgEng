#include <iostream>

#include "matrix.h"
#include "layouts.h"
#include "matrixmul.h"
#include "test.h"

typedef Matrix<RowBased<double>, Naive> RN;
typedef Matrix<ColumnBased<double>, Naive> CN;
typedef Matrix<ZCurve<double>, Naive> ZN;

typedef Matrix<RowBased<double>, Recursive<4>> RR;
typedef Matrix<ZCurve<double>, Recursive<4>> RRZ;

typedef Matrix<RowTiled<4, 4, double>, Recursive<4>> RTR;
typedef Matrix<ColumnTiled<4, 4, double>, Recursive<4>> CTR;

typedef Matrix<ColumnBased<double>, Recursive<4>> CR;
typedef Matrix<ZCurve<double>, Recursive<4>> ZR;

typedef Matrix<RowBased<double>, ParallelNaive<4>> RP;
typedef Matrix<ColumnBased<double>, ParallelNaive<4>> CP;

using namespace std;

int main(int argc, char *argv[]) {
  cout.precision(8);

  sanity_check<RN>();
  sanity_check<CN>();
  sanity_check<RP>();
  sanity_check<ZR>();
  sanity_check<RTR>();
  sanity_check<CTR>();

  // Test 1 (Row column)
  //test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  //test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);

  // Test 2 (Z-curve)
  //test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  //test<ZN,ZN,ZN>(cout, 1, 1024, 1024 * 1024 * 1024);

  // Test 3 (Recursive)
  /*test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RR,RR,RR>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RR,CR,RR>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<ZR,ZR,ZR>(cout, 1, 1024, 1024 * 1024 * 1024);*/

  // Test 4 (Parallel row column)
  /*test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RP,CP,RP>(cout, 1, 1024, 1024 * 1024 * 1024);*/

  return 0;
}
