#include <stdio.h>
#define LEN 1024
#include <stddef.h>
int main()
{
  int x;
  
  unsigned int  a = -1;
  unsigned char b = -1;

  x = (a == b);
  printf("%d\n", x);
}
