#ifndef _ASSERTION_H
#define _ASSERTION_H
#include <stdio.h>

extern int test_count;

#define ASSERT_EQUAL(found, wanted) { \
  do \
  { \
    test_count++; \
    if ((found) != (wanted)) { \
      printf("Assertion failed\nFound: %s\nWanted: %s\n", #found, #wanted); \
    } \
  } while(0); \
}

#define PRINT_EXIT(C, ...) printf(__VA_ARGS__); exit(C);

#define PRINT_RETURN(R, ...) printf(__VA_ARGS__); return(R);

#endif
