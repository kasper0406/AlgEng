#include <iostream>

#ifndef _WINDOWS
  #define Template template
#else
  #define Template
#endif

#include "matrix.h"
#include "layouts.h"
#include "matrixmul.h"
#include "test.h"

const size_t B = 8;

typedef Matrix<RowBased<double>, Naive> RN;
typedef Matrix<ColumnBased<double>, Naive> CN;
typedef Matrix<ZCurve<double>, Naive> ZN;

typedef Matrix<RowBased<double>, Recursive<4, GenericBCMultiplier>> RR;
typedef Matrix<ColumnBased<double>, Recursive<4, GenericBCMultiplier>> RC;
typedef Matrix<ZCurve<double>, Recursive<4, GenericBCMultiplier>> RRZ;

typedef Matrix<ZCurve<double>, Recursive<B, ZLayoutBCMultiplier<B>>> RZBC;

typedef Matrix<RowTiled<B, B, double>, Recursive<B, TiledBCMultiplier>> RTR;
typedef Matrix<RowTiled<B, B, double>, Recursive<B, SIMDTiledBCMultiplier>> SIMDRTR;
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, TiledBCMultiplier, 3>> RTRP;
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, SIMDTiledBCMultiplier, 3>> SIMDRTRP;
typedef Matrix<ColumnTiled<B, B, double>, Recursive<B, GenericBCMultiplier>> CTR;

typedef Matrix<ColumnBased<double>, Recursive<4, GenericBCMultiplier>> CR;
typedef Matrix<ZCurve<double>, Recursive<4, GenericBCMultiplier>> ZR;

typedef Matrix<RowBased<double>, ParallelNaive<4>> RP;
typedef Matrix<ColumnBased<double>, ParallelNaive<4>> CP;

typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen<32, FixedTiledBCMultiplier<32>>> ZRTHS;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen<32, FixedTiledBCMultiplier<32>>> ZCTHS;
typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>> SIMDZRTHS;

using namespace std;

int main(int argc, char *argv[]) {
  cout.precision(8);

  sanity_check<RN, RN>();
  sanity_check<CN, CN>();
  sanity_check<RP, RP>();
  sanity_check<ZR, ZR>();
  sanity_check<RTR, CTR>();
  sanity_check<RTRP, CTR>();
  sanity_check<CTR, CTR>();
  sanity_check<RZBC, RZBC>();
  sanity_check<ZRTHS, ZCTHS>();
  sanity_check<SIMDRTR, CTR>();
  sanity_check<SIMDRTRP, CTR>();

  // test<RZBC,RZBC,RZBC>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 64ULL);
  //test<RTR,CTR,RTR>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 64ULL);
  //test<RZBC,RZBC,RZBC>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 64ULL);
  //test<RN,CN,RN>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 64ULL);

  // Test 1 (Row column)
  //test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  //test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);

  // Test 2 (Z-curve)
  /*test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<ZN,ZN,ZN>(cout, 1, 1024, 1024 * 1024 * 1024);*/

  // Test 3 (Recursive)
  /*test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RR,RR,RR>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RR,CR,RR>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<ZR,ZR,ZR>(cout, 1, 1024, 1024 * 1024 * 1024);*/

  // Test 4 (Parallel row column)
  /*test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<RP,CP,RP>(cout, 1, 1024, 1024 * 1024 * 1024);*/

  // Test 5 (Tiled recursive row/column vs. naive row/column)
  /*test<RN,CN,RN>(cout, 1, 1024, 2048ULL * 2048ULL * 2048ULL);
  test<RTR,CTR,RTR>(cout, 1, 1024, 2048ULL * 2048ULL * 2048ULL);*/

  // Test strassen
  //test<RN,RN,RN>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
  // test<RN,CN,RN>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<ZRTHS,ZCTHS,ZRTHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<SIMDZRTHS,ZCTHS,SIMDZRTHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  /*test<RZBC,RZBC,RZBC>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RTR,CTR,RTR>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);*/

  // Rapport tests
  // test<RN,RN,RN>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  // test<RN,CN,RN>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RZBC,RZBC,RZBC>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RTR,CTR,RTR>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<SIMDRTRP,CTR,SIMDRTRP>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<SIMDRTR,CTR,SIMDRTR>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);

  return 0;
}
