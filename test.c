#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>

int main()
{
  int (*x)[];
  int (*y)[3];
  y = x;
}
