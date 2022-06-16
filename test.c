#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

int f()
{
  return 5;
}
int main()
{
  int (*x)() = &f;
  printf("%p\n%p\n", x, f);
}

