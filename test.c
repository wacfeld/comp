#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>
#include <alloca.h>
#include <stddef.h>
char f(char x)
{
  putd(sizeof(x));
  return x+1;
}


int main()
{
  f(500);
  return 0;
}

