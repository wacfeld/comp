#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define putd(x) printf(#x ": %d\n", x)

void strapp(char *dest, int *max, char *src)
{
  int len = strlen(dest) + strlen(src) + 1;
  putd(len);
  if(len > *max)
  {
    dest = realloc(dest, len);
    *max = len;
  }
  strcat(dest, src);
}

int main()
{
  
  int cs_len;
  char *codeseg = malloc(cs_len);
  *codeseg = 0;

  puts(codeseg);
  strapp(codeseg, &cs_len, "hi");
  puts(codeseg);
  strapp(codeseg, &cs_len, "hey");
  puts(codeseg);
  free(codeseg);
  
  return 0;
}

