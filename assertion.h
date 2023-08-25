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

#endif
