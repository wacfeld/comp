/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define putd(x) printf(#x ": %d\n", x)
char *getbits(u_int32_t dat, int size)
{
  int bitsize = size * 8;

  char *s = malloc(bitsize + 2); // number of bits + b + null terminator
  
  // write bits one at a time backwards into s
  for(int i = 1; i <= bitsize; i++)
  {
    s[bitsize - i] = dat % 2 + '0';
    dat >>= 1;
  }

  s[bitsize] = 'b';
  s[bitsize+1] = 0; // null terminate

  return s;
}*/

int main()
{
  long long int x;
  exit(1);
  
  return 0;
}

