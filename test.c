#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)

int hey = 6;
int hello()
{
  hey++;
  return hey;
}
int y = sizeof(hello());

int main()
{
  putd(hey);
  putd(y);
  putd(hey);
  putd(hello());
  putd(hey);
  
  return 0;
}
