#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

int main()
{
  signed int x = -2;
  unsigned char c = x;
  printf("%d\n", c);

  return 0;
}

