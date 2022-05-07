#include <stdio.h>
#include <ctype.h>
#define putd(x) printf(#x ": %d\n", x)
#define putp(x) printf(#x ": %p\n", x)

int f(int * x)
{
  return *x;
}

int main()
{
  int const *x;
  int *y;
  x = y;
}
