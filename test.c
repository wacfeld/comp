#include <stdio.h>
#define putd(x) printf(#x ": %d\n", x)
#include <stdlib.h>

/*int fun(int c)
{
  
  int x = 5;
  int y = 6;
  x += y;
  return x + c;
}*/

/*int g()
{
  return 5;
}*/

/* int x = 6;*/
/*

storage class specifiers:
- static
  - in function: puts variable in .data
  - outside function: restricts scope to this file (translation unit) (private)
- extern
  - outside function: tells the compiler that storage for this variable is reserved elsewhere (default behaviour when no initializer present)
  - inside function: looks for external declaration in same file, and copies it. if can't find external declaration, looks in other files (external linkage)
- auto
  - unused keyword

- typedef int *integerp;
  - not a real storage class
  - nickname

type qualifiers:
- const, volatile
- tell the compiler what is allowed to be done with a variable in storage
- const: may not be modified after initialization
- volatile: may not be optimized as its value may change unexpectedly


type specifiers:
- int, float, double, char, long int, void, etc.
- custom structs, enums, unions


type modifiers:
- put modifiers between identifier and type specifier

[spec|qual|class]*       type modifiers* (ident) type modifiers*

static auto const int const long

*x
x[]
x[3]
x()
x(void)
x(int a)

int x;
int *x;

int *x[3];

int *x[3]()


   */

void f();


void f()
{
  
}
int x;
int main()
{
static const double const long k;
  int *x[3]; /* array of 3 pointers to int */
  int (*y)[3]; /* pointer to array of 3 ints */
  
  printf("%d\n", sizeof(x));
  printf("%d\n", sizeof(y));
  
  return 1;
  
}
