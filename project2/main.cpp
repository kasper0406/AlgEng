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

typedef Matrix<RowBased<double>, Naive, false> RN;
typedef Matrix<ColumnBased<double>, Naive, false> CN;
typedef Matrix<ZCurve<double>, Naive, false> ZN;

typedef Matrix<RowBased<double>, Recursive<4, GenericBCMultiplier>, false> RR;
typedef Matrix<ColumnBased<double>, Recursive<4, GenericBCMultiplier>, false> RC;
typedef Matrix<ZCurve<double>, Recursive<4, GenericBCMultiplier>, false> RRZ;

typedef Matrix<ZCurve<double>, Recursive<B, ZLayoutBCMultiplier<B>>, false> RZBC;

typedef Matrix<RowTiled<B, B, double>, Recursive<B, TiledBCMultiplier>, false> RTR;
typedef Matrix<RowTiled<B, B, double>, Recursive<B, SIMDTiledBCMultiplier>, false> SIMDRTR;
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, TiledBCMultiplier, 2>, false> RTRP;
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, SIMDTiledBCMultiplier, 3>, false> SIMDRTRP;
typedef Matrix<ColumnTiled<B, B, double>, Recursive<B, GenericBCMultiplier>, false> CTR;

typedef Matrix<RowTiled<B, B, double>, TiledIterative<B, TiledBCMultiplier>, false> RTI;
typedef Matrix<ColumnTiled<B, B, double>, TiledIterative<B, TiledBCMultiplier>, false> CTI;
typedef Matrix<RowTiled<B, B, double>, ParallelTiledIterative<B,4,TiledBCMultiplier>, false> RPTI;
typedef Matrix<ColumnTiled<B, B, double>, ParallelTiledIterative<B,4,TiledBCMultiplier>, false> CPTI;

typedef Matrix<RowTiled<B, B, double>, TiledIterative<B, SIMDTiledBCMultiplier>, false> SIMDRTI;
typedef Matrix<ColumnTiled<B, B, double>, TiledIterative<B, SIMDTiledBCMultiplier>, false> SIMDCTI;
typedef Matrix<RowTiled<B, B, double>, ParallelTiledIterative<B,4,SIMDTiledBCMultiplier>, false> SIMDRPTI;
typedef Matrix<ColumnTiled<B, B, double>, ParallelTiledIterative<B,4,SIMDTiledBCMultiplier>, false> SIMDCPTI;

typedef Matrix<ColumnBased<double>, Recursive<4, GenericBCMultiplier>, false> CR;
typedef Matrix<ZCurve<double>, Recursive<4, GenericBCMultiplier>, false> ZR;

typedef Matrix<RowBased<double>, ParallelNaive<4>, false> RP;
typedef Matrix<ColumnBased<double>, ParallelNaive<4>, false> CP;

typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZRTHS;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZCTHS;
typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZRTHS;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZCTHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZRTPHS;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZCTPHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZRTPHSS;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZCTPHSS;

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
  sanity_check<ZRTPHS, ZCTPHS>();
  sanity_check<SIMDRTR, CTR>();
  sanity_check<RTI, CTI>();
  sanity_check<RPTI, CPTI>();
  sanity_check<RTRP, CTR>();
  sanity_check<SIMDZRTPHSS, SIMDZCTPHSS>();

  ////// Rapport tests
  const int trials = 1;
  const size_t min_size = 1024 * 1024;
  const size_t max_size = 1024ULL * 1024ULL * 1024ULL * 64ULL;
  
  // Sequantial
  test<RN,RN,RN>(cout, trials, min_size, max_size);
  test<RN,CN,RN>(cout, trials, min_size, max_size);
  test<RR,RC,RR>(cout, trials, min_size, max_size);
  test<RRZ,RRZ,RRZ>(cout, trials, min_size, max_size);
  test<RZBC,RZBC,RZBC>(cout, trials, min_size, max_size);
  test<RTR,CTR,RTR>(cout, trials, min_size, max_size);
  test<SIMDRTR,CTR,SIMDRTR>(cout, trials, min_size, max_size);
  test<RTI,CTI,RTI>(cout, trials, min_size, max_size);
  test<SIMDRTI,SIMDCTI,SIMDRTI>(cout, trials, min_size, max_size);
  test<ZRTHS,ZCTHS,ZRTHS>(cout, trials, min_size, max_size);
  test<SIMDZRTHS,SIMDZCTHS,SIMDZRTHS>(cout, trials, min_size, max_size);

  // Parallel
  test<RP,CP,RP>(cout, trials, min_size, max_size);
  test<RTRP,CTR,RTRP>(cout, trials, min_size, max_size);
  test<SIMDRTRP,CTR,SIMDRTRP>(cout, trials, min_size, max_size);
  test<RPTI,CPTI,RPTI>(cout, trials, min_size, max_size);
  test<SIMDRPTI,SIMDCPTI,SIMDRPTI>(cout, trials, min_size, max_size);
  test<ZRTPHS,ZCTPHS,ZRTPHS>(cout, trials, min_size, max_size);
  test<SIMDZRTPHSS,SIMDZCTPHSS,SIMDZRTPHSS>(cout, trials, min_size, max_size);
  
  return 0;
}
