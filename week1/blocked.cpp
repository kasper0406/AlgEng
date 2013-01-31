#include "stdio.h"
#include "stdlib.h"
#include "math.h"

const int B = 7;
const int d = B + 1;

void build(int* arr, int pos, int* numbers, int start, int end)
{  
  // printf("build(arr, %d, numbers, %d, %d)\n", pos, start, end);
  if (start > end) return; // Should not happen...
  if (start + B - 1 == end) {
    for (int i = 0; i < B; i++)
      arr[pos + i] = numbers[start + i];
     return;
  }

  const int parentleaf = pos / B;
  // printf("parent leaf = %d\n", parentleaf);

  int prevIndex = start - 1;
  int curleaf;
  for (int i = 1; i < d; i++) {
    curleaf = d * parentleaf + i;
    // printf("cur leaf = %d\n", curleaf);

    int index = start + ((double)(end - start + 1) / d) * i;

    // printf("index = %d\n", index);
    arr[B * parentleaf + (i - 1)] = numbers[index];
    build(arr, B * curleaf, numbers, prevIndex + 1, index - 1);

    prevIndex = index;
  }

  curleaf = d * (parentleaf + 1);
  // printf("cur leaf = %d\n", curleaf);
  build(arr, B * curleaf, numbers, prevIndex + 1, end);
}

int bs_scan_search(int q, int* arr, int block)
{
  for (int i = 0; i < B; i++) {
    if (arr[block + i] > q) {
      return bs_scan_search(q, arr, ...);
    } else if (arr[block + i] < q) {
      if (i == B - 1)
        return bs_scan_search(q, arr, ...);
    } else {
      // equal
      return block + i;
    }
  }
}

int main(int argc, char* argv[])
{
  int i = 7;
  int n = pow(d, i) - 1;
  int* arr = (int*) malloc(n * sizeof(int));
  int* numbers = (int*) malloc(n * sizeof(int));

  for (int i = 0; i < n; i++) {
    numbers[i] = i;
    // printf("%5d | ", numbers[i]);
  }
  // printf("\n");
  build(arr, 0, numbers, 0, n - 1);

  printf("First block:\n");
  for (int i = 0; i < B; i++)
    printf("%10d", arr[i]);
  printf("\n");

  //for (int i = 0; i < n; i++)
    // printf("%5d | ", arr[i]);
  // printf("\n");

  return 0;
}
