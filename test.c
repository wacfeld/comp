#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>
#include <alloca.h>
#include <stddef.h>

int main()
{
  int a[5];
  putd(sizeof(a[-1]));
  return 0;
}

