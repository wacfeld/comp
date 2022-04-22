#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)
int f(int x)
{
  return 2 * x;
}
int main()
{
 putd(f(3) + 1);
  return 0;
}
