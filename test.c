#include <stdio.h>
#define LEN 1024
#include <stddef.h>
int main()
{
  int c;
  
  int a = 0x123456;
  int b = 0x00ffff;
  c = a ^ b;
  printf("%x\n", c);
}
