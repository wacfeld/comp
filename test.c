#include <stdio.h>
#include <ctype.h>
#define putd(x) printf(#x ": %d\n", x)


int main()
{
  char s[] = "hellohellohello";
  putd(sizeof(s));
  putd(sizeof(s+1));
}
