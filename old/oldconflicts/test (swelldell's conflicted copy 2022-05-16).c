#include <stdio.h>
#include <ctype.h>
#define putd(x) printf(#x ": %d\n", x)
#define putp(x) printf(#x ": %p\n", x)




int main()
{
  struct x;

  struct x{int a;};
  struct x y;

  return 0;
}
