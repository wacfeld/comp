#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>


int main()
{
  int x = 6;
  {
    extern int x;
  putd(x);
  }
  return 0;
}
int x = 5;
