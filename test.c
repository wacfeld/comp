#include <stdio.h>


int main()
{
  int zzz = (25*26 + 25)*26 + 25;
  printf("%d\n", zzz);
  
  int cur = 0;
  while(cur <= zzz)
  {
    int d1 = cur /26 /26;
    int d2 = cur /26 %26;
    int d3 = cur %26;

    printf("%c%c%c\n", d1+'A', d2+'A', d3+'A');
    
    cur++;
  }
}
