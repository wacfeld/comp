#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>

void g()
{
  int a = 14;;
}

void f()
{
  int x = 6;
  {
    int y = 7;
    {
      int z = 8;
      if(y == 7)
      {
        int k = 9;
        g();
      }
      else
      {
        int k = 10;
        g();
      }
    }
  }
  
  g();
}

int main()
{
  return 0;
}
