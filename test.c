#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)
int f(int x)
{
  return 2 * x;
}
int main()
{
  putd(sizeof -5);
}
