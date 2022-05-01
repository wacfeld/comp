#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)



int len = 5;
int main()
{
len += 3;
  
  int x[len];
  putd(sizeof(x));
}
