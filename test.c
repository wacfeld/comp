#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)

int x;

void f()
{
  putd(x);
}
int x = 5;
x;
int main()
{
  f();
  return 0;
}
