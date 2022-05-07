#include <stdio.h>
#include <ctype.h>
#define putd(x) printf(#x ": %d\n", x)
#define putp(x) printf(#x ": %p\n", x)




int main()
{
  const int *x;
  int *y;
  y = x;
  return 0;
}
