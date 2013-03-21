#include <iostream>
#include <functional>

#ifndef _WINDOWS
  #define Template template
#else
  #define Template
#endif

#define MB (1024 * 1024)
#define _STACKALLOC 1024 * MB
bool stack_allocate = false;

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
typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZRTHS2;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZCTHS2;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZRTPHS;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZCTPHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZRTPHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZRTPHS2;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZCTPHS2;

typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZRTPHSS;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZCTPHSS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen2<32, SIMDFixedTiledBCMultiplier2<32>>, true> SIMDZRTPHSS2;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen2<32, SIMDFixedTiledBCMultiplier2<32>>, true> SIMDZCTPHSS2;

using namespace std;

void avoid_stack_allocation(function<void()> a) {
  cout << endl << "### Stack allocation disabled." << endl;
  stack_allocate = false;
  a();
};

void stack_allocation(function<void()> a) {
#ifdef _STACKALLOC
  cout << endl << "### Stack allocation enabled." << endl;
  stack_allocate = true;

  a();
#else
  avoid_stack_allocation(a);
#endif
};

int main(int argc, char *argv[]) {
  cout.precision(8);

  try {
    stack_allocation([] () {
      sanity_check<RN, CN>();
      sanity_check<ZRTHS2, ZCTHS2>();
    });
    avoid_stack_allocation([] () {
      sanity_check<ZRTHS, ZCTHS>();
      sanity_check<ZRTPHS2, ZCTPHS2>();
    });

    stack_allocation([] () {
      test<RN, CN, RN>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
      test<ZRTHS2, ZCTHS2, ZRTHS2>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
    });
    avoid_stack_allocation([] () {
      test<ZRTHS2, ZCTHS2, ZRTHS2>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
      test<ZRTHS, ZCTHS, ZRTHS>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
      test<ZRTPHS2, ZCTPHS2, ZRTPHS2>(cout, 1, 1024 * 1024, 1024ULL * 1024ULL * 1024ULL);
    });
  } catch (logic_error err) {
    cout << err.what() << endl;
  }

  return 0;
}
