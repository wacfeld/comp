#include <stdio.h>
#define LEN 1024
#include <stddef.h>

int main()
{
  int a;
  void *x = &a;
  void *p = x+1;
}
