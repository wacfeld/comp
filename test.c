#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>
#include <alloca.h>


int main()
{
  int y = 5;
  int x[y];
  int i;
  for(i = 0; i < 6; i++)
  {
    putd(x[i]);
  }
  return 0;
}
