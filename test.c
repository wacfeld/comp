#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>
#include <alloca.h>
#include <stddef.h>


int main()
{
  float f = -1.40129846432e-45;
  int c = 0;
  
  memcpy(&c, &f, 4);
  printf("%x\n", c);
  return 0;
}

