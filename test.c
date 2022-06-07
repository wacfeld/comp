#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>


int main()
{
  int x = 5;
  int y = 31;
  printf("%d\n",x << y);
  return 0;
}
