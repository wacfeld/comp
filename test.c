#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)
x = 5;
int main()
{
  putd(__LINE__);
}
