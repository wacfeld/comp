#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
int main()
{
  int x[5] = {0,0,0,0,0};
  int (*y)[] = &x;
  printf("%d\n", *y);
}

