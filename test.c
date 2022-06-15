#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
enum gpreg {EAX=0, EBX=1, ECX=2, EDX=3};

char *gpr_ref[4][5] =
{
  [EAX] =
  {
    [1] = "AL",
    [2] = "AX",
    [4] = "EAX",
  },
  [EBX] =
  {
    [1] = "BL",
    [2] = "BX",
    [4] = "EBX",
  },
  [ECX] =
  {
    [1] = "CL",
    [2] = "CX",
    [4] = "ECX",
  },
  [EDX] =
  {
    [1] = "DL",
    [2] = "DX",
    [4] = "EDX",
  },
};

char *regstr(enum gpreg gpr, int size)
{
  assert(size == 1 || size == 2 || size == 4);
  return gpr_ref[gpr][size];
}
int main()
{
  puts(regstr(EBX, 1));
}

