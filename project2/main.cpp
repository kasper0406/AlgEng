#include <iostream>

#include "matrix.h"
#include "layouts.h"
#include "matrixmul.h"
#include "test.h"

typedef Matrix<RowBased<int32_t>, Naive> M0;
typedef Matrix<ColumnBased<double>, Naive> M1;

using namespace std;

int main(int argc, char *argv[]) {
  cout.precision(8);
  /*{
    M0 a(2, 3);
    M1 b(3, 2);

    a(0, 0) = 1;
    a(0, 1) = 2;
    a(0, 2) = 3;
    a(1, 0) = 4;
    a(1, 1) = 5;
    a(1, 2) = 6;

    b(0, 0) = 6;
    b(0, 1) = 5;
    b(1, 0) = 4;
    b(1, 1) = 3;
    b(2, 0) = 2;
    b(2, 1) = 1.97;
    cout << "inden *" << endl;
    M0 c = a.operator*<M1, M0>(b);
    cout << "main end" << endl;
  }*/
  test<M0,M1,M0>(cout, 5, 1024, 1024 * 1024 * 1024);
  cout << "done" << endl;

  return 0;
}
