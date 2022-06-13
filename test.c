#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define putd(x) printf(#x ": %d\n", x)

int main()
{
  signed char c = -5;
  unsigned char x = c;
  putd(x);
  
  return 0;
}

