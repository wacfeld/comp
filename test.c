#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getoffstr(int off)
{
  // largest displacement is 32 bits -> 10 decimal digits. add on sign and null term, 12 digits
  char *s = malloc(12);

  // sign
  if(off >= 0)
    s[0] = '+';
  else
  {
    s[0] = '-';
    off = -off; // take absolute value
  }

  // write number in after sign
  sprintf(s+1, "%d", off);

  return s;
}

int main()
{
  printf("%s\n", getoffstr(0));
  
  return 0;
}

