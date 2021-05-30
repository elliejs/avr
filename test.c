#include <stdio.h>


int main(int argc, char const *argv[]) {
  int a[] = {0,1,2};
  int *b = &a[1];
  printf("%d\n", b[1]);
  return 0;
}
