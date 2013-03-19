#include <iostream>

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
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, TiledBCMultiplier, 2>> RTRP;
typedef Matrix<ColumnTiled<B, B, double>, Recursive<B, GenericBCMultiplier>> CTR;

typedef Matrix<ColumnBased<double>, Recursive<4, GenericBCMultiplier>> CR;
typedef Matrix<ZCurve<double>, Recursive<4, GenericBCMultiplier>> ZR;

typedef Matrix<RowBased<double>, ParallelNaive<4>> RP;
typedef Matrix<ColumnBased<double>, ParallelNaive<4>> CP;

typedef Matrix<RowBased<double>, Strassen<32, GenericBCMultiplier>> RS;
typedef Matrix<RowBased<double>, FakeStrassen<32, GenericBCMultiplier>> RF;
typedef Matrix<ColumnBased<double>, Strassen<32, GenericBCMultiplier>> CS;
typedef Matrix<ColumnBased<double>, FakeStrassen<32, GenericBCMultiplier>> CF;
typedef Matrix<ZCurve<double>, Strassen<32, ZLayoutBCMultiplier<32>>> ZS;
typedef Matrix<CachingZCurve<double>, Strassen<32, GenericBCMultiplier>> CZS;
typedef Matrix<RowTiled<32, 32, double>, Strassen<32, TiledBCMultiplier>> RTS;
typedef Matrix<ColumnTiled<32, 32, double>, Strassen<32, TiledBCMultiplier>> CTS;

using namespace std;

int main(int argc, char *argv[]) {
  cout.precision(8);
  
  /*sanity_check<RN, RN>();
  sanity_check<CN, CN>();
  sanity_check<RP, RP>();
  sanity_check<ZR, ZR>();
  sanity_check<RTR, CTR>();
  sanity_check<RTRP, CTR>();
  sanity_check<CTR, CTR>();
  sanity_check<RZBC, RZBC>();
  sanity_check<RS, RS>();
  test<RP,CP,RP>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
  test<RTRP,CTR,RTRP>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
  sanity_check<RTS, CTS>();*/
  sanity_check<CZS, CZS>();

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
  test<RN,CN,RN>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RS,RS,RS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RTS,CTS,RTS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RS,CS,RS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<ZS,ZS,ZS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<CZS,CZS,CZS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RZBC,RZBC,RZBC>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RTR,CTR,RTR>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);

  // Rapport tests
  // test<RN,RN,RN>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  // test<RN,CN,RN>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  // test<RZBC,RZBC,RZBC>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  // test<RTR,CTR,RTR>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);

  return 0;
}
