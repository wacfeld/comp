#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>

int main()
{
  int x;
  int *y = &x;
  (*((const int *) y))++;
  (*y)++;
}
