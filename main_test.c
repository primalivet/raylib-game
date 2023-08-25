#include <stdio.h>
#include "assertion.h"

int test_count = 0;

int main() {
  test_count++;
  printf("completed, ran %d tests\n", test_count);
  return 0;
}
