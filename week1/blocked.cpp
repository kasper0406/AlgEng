#include "stdio.h"
#include "stdlib.h"
#include "math.h"

const int B = 2;
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

void print_block(int* arr, int block)
{
  if (block == 0)
    printf("1st block:\n");
  else
    printf("%dth block:\n", block + 1);

  for (int i = 0; i < B; i++)
    printf("%10d", arr[block * B + i]);
  printf("\n");
}

int bs_scan_search_rec(int q, int* arr, int n, int block)
{
  // printf("bs_scan_search_rec(%d, arr, %d);\n", q, block);
  // print_block(arr, block);

  for (int i = 0; i < B; i++) {
    const int index = B * block + i;
    if (arr[index] > q) {
      int newblock = d * block + 1 + i;
      if (newblock * B >= n) {
        return (i > 0) ? index - 1 : -1;
      } else {
        int res_index = bs_scan_search_rec(q, arr, n, newblock);
        if (res_index != -1)
          return res_index;
        else
          return (i > 0) ? index - 1 : -1;
      }
    } else if (arr[index] < q) {
      if (i == B - 1) {
        // Check if we are out of bounds
        int newblock = d * (block + 1);
        if (newblock * B >= n) {
          return index;
        } else {
          int res_index = bs_scan_search_rec(q, arr, n, newblock);
          if (res_index != -1)
            return res_index;
          else
            return index;
        }
      }
    } else {
      // equal
      return index;
    }
  }
  return -1;
}

int bs_scan_search_iter(int q, int* arr, int n, int block)
{
  int cur_answer = -1;

  // While we are not out of bounds we search the block
  while (B * block < n) {
    // Make a linear scan of the block
    for (int i = 0; i < B; i++) {
      const int index = B * block + i;
      if (q < arr[index]) {
        if (i > 0)
          cur_answer = index - 1;
        block = d * block + 1 + i;
        break;
      } else if (arr[index] < q) {
        cur_answer = index;
        if (i == B - 1) {
          block = d * (block + 1);
          break;
        }
      } else {
        // equal
        return index;
      }
    }
  }

  return cur_answer;
}

int main(int argc, char* argv[])
{
  int i = 3;
  int n = pow(d, i) - 1;
  int* arr = (int*) malloc(n * sizeof(int));
  int* numbers = (int*) malloc(n * sizeof(int));

  printf("n = %d\n", n);

  for (int i = 0; i < n; i++) {
    numbers[i] = 3 * i;
    // printf("%5d | ", numbers[i]);
  }
  // printf("\n");
  build(arr, 0, numbers, 0, n - 1);

  print_block(arr, 0);

  for (int q = -1; q < 3 * n; q++) {
    int index = bs_scan_search_iter(q, arr, n, 0);
    if (index != -1)
      printf("query(%d) = arr[%d] = %d\n", q, index, arr[index]);
    else
      printf("query(%d) = no solution\n", q);
  }

  return 0;
}
