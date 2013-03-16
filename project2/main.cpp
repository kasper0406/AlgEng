#include <iostream>

#include "matrix.h"
#include "layouts.h"
#include "matrixmul.h"
#include "test.h"

typedef Matrix<RowBased<double>, Naive> RN;
typedef Matrix<ZCurve<double>, Naive> RZ;
typedef Matrix<ColumnBased<double>, Naive> CN;

typedef Matrix<RowBased<double>, Recursive<4>> RR;

using namespace std;

int main(int argc, char *argv[]) {
  /*
  cout.precision(8);
  {
    Mrecursive a(2, 3);
    Mrecursive b(3, 2);

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
    Mrecursive c = a.operator*<Mrecursive, Mrecursive>(b);
    cout << "main end" << endl;
    
    cout << c(0,0) << "\t" << c(0,1) << endl
         << c(1,0) << "\t" << c(1,1) << endl;
  }
   */
  
  test<RR,RR,RR>(cout, 5, 32*32*32, 1024 * 1024 * 1024);
  cout << "done" << endl;
  
  // Test 1
  //test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  //test<RN,CN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);

  // Test 2
  //test<RN,RN,RN>(cout, 1, 1024, 1024 * 1024 * 1024);
  //test<RZ,RZ,RZ>(cout, 1, 1024, 1024 * 1024 * 1024);

  return 0;
}
