#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>

void f(float a)
{
  printf("%f\n", a);
}

int main()
{
  f(5);
}
