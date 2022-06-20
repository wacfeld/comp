#include <stdio.h>
#define LEN 1024
#include <stddef.h>


const int *f()
{
 volatile int *x = 0;
  return x;
}
int main()
{
}
