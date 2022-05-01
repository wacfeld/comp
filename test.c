#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)


int *volatile x;
int * x;

int main()
{
  return 0;
}
