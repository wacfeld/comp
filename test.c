#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>

int main()
{
  int *a;
  int const * const b;
  (0 ? a : b);
}
