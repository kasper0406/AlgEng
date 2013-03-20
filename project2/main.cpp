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
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, TiledBCMultiplier, 2>> RTRP;
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, SIMDTiledBCMultiplier, 3>> SIMDRTRP;
typedef Matrix<ColumnTiled<B, B, double>, Recursive<B, GenericBCMultiplier>> CTR;

typedef Matrix<RowTiled<B, B, double>, TiledIterative<B>> RTI;
typedef Matrix<ColumnTiled<B, B, double>, TiledIterative<B>> CTI;
typedef Matrix<RowTiled<B, B, double>, ParallelTiledIterative<B,4>> RPTI;
typedef Matrix<ColumnTiled<B, B, double>, ParallelTiledIterative<B,4>> CPTI;

typedef Matrix<ColumnBased<double>, Recursive<4, GenericBCMultiplier>> CR;
typedef Matrix<ZCurve<double>, Recursive<4, GenericBCMultiplier>> ZR;

typedef Matrix<RowBased<double>, ParallelNaive<4>> RP;
typedef Matrix<ColumnBased<double>, ParallelNaive<4>> CP;

typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen<32, FixedTiledBCMultiplier<32>>> ZRTHS;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen<32, FixedTiledBCMultiplier<32>>> ZCTHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>> ZRTPHS;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>> ZCTPHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>> ZRTPHS;

typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>> SIMDZRTPHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZRTPHSS;

using namespace std;

int main(int argc, char *argv[]) {
  cout.precision(8);

  ///*sanity_check<RN, RN>();
  //sanity_check<CN, CN>();
  //sanity_check<RP, RP>();
  //sanity_check<ZR, ZR>();
  //sanity_check<RTR, CTR>();
  //sanity_check<RTRP, CTR>();
  //sanity_check<CTR, CTR>();
  //sanity_check<RZBC, RZBC>();
  //sanity_check<ZRTHS, ZCTHS>();*/
  //sanity_check<ZRTPHS, ZCTPHS>();
  /////*sanity_check<SIMDRTR, CTR>();*/
  
  sanity_check<RTR, CTR>();
  sanity_check<RTI, CTI>();
  sanity_check<RPTI, CPTI>();
  sanity_check<RTRP, CTR>();
  sanity_check<SIMDZRTPHS, ZCTPHS>();
  sanity_check<SIMDZRTPHSS, ZCTPHS>();

  test<SIMDZRTPHS,ZCTPHS,SIMDZRTPHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<SIMDZRTPHSS,ZCTPHS,SIMDZRTPHSS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  
  test<RTR,CTR,RTR>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
  test<RTI,CTI,RTI>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
  test<RP,CP,RP>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
  test<RPTI,CPTI,RPTI>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
  test<RTRP,CTR,RTRP>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);


  /////*test<RTR,CTR,RTR>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  ////test<SIMDRTR,CTR,SIMDRTR>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);*/


//  test<ZRTPHS,ZCTPHS,ZRTPHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
//  test<RN,CN,RN>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
//  test<ZRTHS,ZCTHS,ZRTHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
//  test<RP,CP,RP>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);

  /*test<ZRTPHS,ZCTPHS,ZRTPHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RN,CN,RN>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<ZRTHS,ZCTHS,ZRTHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  test<RP,CP,RP>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);*/
  
  ////// Rapport tests
  ////// test<RN,RN,RN>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  ////// test<RN,CN,RN>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  ////// test<RZBC,RZBC,RZBC>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  ////// test<RTR,CTR,RTR>(cout, 3, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);
  ////// test<ZRTHS,ZCTHS,ZRTHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL * 8ULL);

  return 0;
}
