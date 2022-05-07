#include <stdio.h>
#include <ctype.h>
#define putd(x) printf(#x ": %d\n", x)
#define putp(x) printf(#x ": %p\n", x)

int main()
{
  typedef struct p p;
  struct p {int x;};
}
