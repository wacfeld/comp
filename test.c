#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>
#include <alloca.h>
#include <stddef.h>

int f(const int x)
{
  x++;
  return x;
}

int main()
{
  putd(f(6));
  return 0;
}

