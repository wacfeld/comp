#include <stdio.h>
#include <ctype.h>
#define putd(x) printf(#x ": %d\n", x)
#define putp(x) printf(#x ": %p\n", x)




int main()
{
  int (*f)(int *volatile x);
  int (*g)(int *x);
  g = f;
  return 0;
}
