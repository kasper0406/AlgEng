#include "stdio.h"
#include "stdlib.h"

inline int bs(int q, int* arr, int n)
{
  int min, max, mid;
  min = 0;
  max = n - 1;

  while (min <= max) {
    mid = (max + min) / 2;
    if (arr[mid] < q)
      min = mid + 1;
    else if (arr[mid] > q)
      max = mid - 1;
    else
      return arr[mid];
  }
  return arr[max];
}

int main(int argc, char* argv[])
{
  int n = argv[1];
  int[] q = { 1, 6, 8, 22, 5, 7 };
  const int qlen = sizeof(q) / sizeof(int);

  int* arr = (int*) malloc(n * sizeof(int));
  for (int i = 0; i < n; i++)
    arr[i] = 2 * i;

  int res = -1;
  for (int i = 0; i < qlen; i++)
    res = bs(q, arr, n);
  printf("res = %d\n", res);

  return 0;
}
