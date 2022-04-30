#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)

int main()
{
  
  int i = sizeof(long double []);
  putd(i);
}
