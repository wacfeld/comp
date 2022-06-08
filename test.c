#include <stdio.h>
#include <assert.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>
#include <alloca.h>
#include <stddef.h>


int main()
{
  putd(sizeof(ptrdiff_t));
  return 0;
}
