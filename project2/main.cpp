#include <iostream>

#include "matrix.h"
#include "layouts.h"
#include "matrixmul.h"
#include "test.h"

typedef Matrix<RowBased<double>, Naive> MR;
typedef Matrix<ColumnBased<double>, Naive> MC;

using namespace std;

int main(int argc, char *argv[]) {
  cout.precision(8);
  
  // Test 1
  test<MR,MR,MR>(cout, 1, 1024, 1024 * 1024 * 1024);
  test<MR,MC,MR>(cout, 1, 1024, 1024 * 1024 * 1024);

  return 0;
}
