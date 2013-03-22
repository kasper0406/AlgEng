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
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, TiledBCMultiplier, 3>, false> RTRP;
typedef Matrix<RowTiled<B, B, double>, ParallelRecursive<B, SIMDTiledBCMultiplier, 3>, false> SIMDRTRP;
typedef Matrix<ColumnTiled<B, B, double>, Recursive<B, GenericBCMultiplier>, false> CTR;

typedef Matrix<RowTiled<B, B, double>, TiledIterative<B, TiledBCMultiplier>, false> RTI;
typedef Matrix<ColumnTiled<B, B, double>, TiledIterative<B, TiledBCMultiplier>, false> CTI;
typedef Matrix<RowTiled<B, B, double>, ParallelTiledIterative<B,8,TiledBCMultiplier>, false> RPTI;
typedef Matrix<ColumnTiled<B, B, double>, ParallelTiledIterative<B,8,TiledBCMultiplier>, false> CPTI;

typedef Matrix<RowTiled<B, B, double>, TiledIterative<B, SIMDTiledBCMultiplier>, false> SIMDRTI;
typedef Matrix<ColumnTiled<B, B, double>, TiledIterative<B, SIMDTiledBCMultiplier>, false> SIMDCTI;
typedef Matrix<RowTiled<B, B, double>, ParallelTiledIterative<B,8,SIMDTiledBCMultiplier>, false> SIMDRPTI;
typedef Matrix<ColumnTiled<B, B, double>, ParallelTiledIterative<B,8,SIMDTiledBCMultiplier>, false> SIMDCPTI;

typedef Matrix<ColumnBased<double>, Recursive<4, GenericBCMultiplier>, false> CR;
typedef Matrix<ZCurve<double>, Recursive<4, GenericBCMultiplier>, false> ZR;

typedef Matrix<RowBased<double>, ParallelNaive<8>, false> RP;
typedef Matrix<ColumnBased<double>, ParallelNaive<8>, false> CP;

typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZRTHS;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZCTHS;
typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZRTHS2;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZCTHS2;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZRTPHS;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen<32, FixedTiledBCMultiplier<32>>, false> ZCTPHS;
typedef Matrix<ZCurveTiled<double, 32, true>, ParallelHackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZRTPHS2;
typedef Matrix<ZCurveTiled<double, 32, false>, ParallelHackyStrassen2<32, FixedTiledBCMultiplier2<32>>, false> ZCTPHS2;

typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZRTHS;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen<32, SIMDFixedTiledBCMultiplier<32>>, true> SIMDZCTHS;
typedef Matrix<ZCurveTiled<double, 32, true>, HackyStrassen2<32, SIMDFixedTiledBCMultiplier2<32>>, true> SIMDZRTHS2;
typedef Matrix<ZCurveTiled<double, 32, false>, HackyStrassen2<32, SIMDFixedTiledBCMultiplier2<32>>, true> SIMDZCTHS2;

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

#ifndef NDEBUG
  cout << "Assertions are enabled!!!!!!!!!!!!!!" << endl;
#endif

  try {
    // Sequential
    avoid_stack_allocation([] () {
	    sanity_check<RN, RN>();
	    sanity_check<CN, CN>();
	    sanity_check<ZR, ZR>();
	    sanity_check<RTR, CTR>();
	    sanity_check<CTR, CTR>();
	    sanity_check<RZBC, RZBC>();
	    sanity_check<SIMDRTR, CTR>();
	    sanity_check<RTI, CTI>();
    });
    stack_allocation([] () {
      sanity_check<ZRTHS2, ZCTHS2>();
      sanity_check<SIMDZRTHS2, SIMDZCTHS2>();
    });
    // Parallel
    avoid_stack_allocation([] () {
	    sanity_check<RP, RP>();
	    sanity_check<RTRP, CTR>();
	    sanity_check<ZRTHS, ZCTHS>();
	    sanity_check<ZRTPHS, ZCTPHS>();
      sanity_check<SIMDZRTHS,SIMDZCTHS>();
      sanity_check<ZRTPHS2, ZCTPHS2>();
	    sanity_check<RPTI, CPTI>();
	    sanity_check<RTRP, CTR>();
	    sanity_check<SIMDZRTPHSS, SIMDZCTPHSS>();
	    sanity_check<SIMDZRTPHSS2, SIMDZCTPHSS2>();
    });

    // Rapport tests
    const int trials = 1;
    const size_t min_size = 1024 * 1024;
    const uint64_t max_size = 1024ULL * 1024ULL * 1024ULL * 64ULL;
	
	  // Sequential
   // avoid_stack_allocation([&] () {
	  //  //test<RN,RN,RN>(cout, trials, min_size, max_size);
   //   test<RN,CN,RN>(cout, trials, min_size, max_size);
	  //  test<RR,RC,RR>(cout, trials, min_size, max_size);
	  //  test<RRZ,RRZ,RRZ>(cout, trials, min_size, max_size);
	  //  test<RZBC,RZBC,RZBC>(cout, trials, min_size, max_size);
	  //  test<RTR,CTR,RTR>(cout, trials, min_size, max_size);
	  //  test<SIMDRTR,CTR,SIMDRTR>(cout, trials, min_size, max_size);
	  //  test<RTI,CTI,RTI>(cout, trials, min_size, max_size);
	  //  test<SIMDRTI,SIMDCTI,SIMDRTI>(cout, trials, min_size, max_size);
	  //  test<ZRTHS2,ZCTHS2,ZRTHS2>(cout, trials, min_size, max_size);
	  //  test<SIMDZRTHS2,SIMDZCTHS2,SIMDZRTHS2>(cout, trials, min_size, max_size);
   //   test<ZRTHS,ZCTHS,ZRTHS>(cout, trials, min_size, max_size);
	  //  test<SIMDZRTHS,SIMDZCTHS,SIMDZRTHS>(cout, trials, min_size, max_size);
   // });
   // stack_allocation([&] () {
	  //  test<ZRTHS2,ZCTHS2,ZRTHS2>(cout, trials, min_size, max_size);
	  //  test<SIMDZRTHS2,SIMDZCTHS2,SIMDZRTHS2>(cout, trials, min_size, max_size);
	  //});

    // Parallel
	  avoid_stack_allocation([&] () {
	    test<RP,CP,RP>(cout, trials, min_size, max_size);
	    test<RTRP,CTR,RTRP>(cout, trials, min_size, max_size);
	    test<SIMDRTRP,CTR,SIMDRTRP>(cout, trials, min_size, max_size);
	    test<RPTI,CPTI,RPTI>(cout, trials, min_size, max_size);
	    test<SIMDRPTI,SIMDCPTI,SIMDRPTI>(cout, trials, min_size, max_size);
	    test<ZRTPHS,ZCTPHS,ZRTPHS>(cout, trials, min_size, max_size);
	    test<ZRTPHS2, ZCTPHS2, ZRTPHS2>(cout, 1, min_size, max_size);
	    test<SIMDZRTPHSS,SIMDZCTPHSS,SIMDZRTPHSS>(cout, trials, min_size, max_size);
	    test<SIMDZRTPHSS2,SIMDZCTPHSS2,SIMDZRTPHSS2>(cout, trials, min_size, max_size);
    });
  } catch (logic_error err) {
    cout << err.what() << endl;
  }
  return 0;
}
