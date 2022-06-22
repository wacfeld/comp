#include <stdio.h>
// #include <assert.h>
#include <ctype.h>
// #include <errno.h>
// #include <float.h>
// #include <limits.h>
// #include <locale.h>
// #include <math.h>
// #include <setjmp.h>
// #include <signal.h>
// #include <stdarg.h>
// #include <stddef.h>
// #include <stdlib.h>
#include <string.h>
// #include <time.h>
#define putd(x) printf(#x ": %d\n", x)


void dcl(char *str)
{
  // skip beginning whitespace
  while(*str == ' ' && *str != 0)
    str++;

  int len = strlen(str);

  if(*str == '*') // pointer
  {
    str++;
    dcl(str);
    printf("pointer to ");
  }
  else if(str[len-1] == ')' && str[len-2] == '(') // function
  {
    str[len-2] = 0;
    dcl(str);
    printf("function returning ");
  }
  else if(str[len-1] == ']' && str[len-2] == '[') // array
  {
    str[len-2] = 0;
    dcl(str);
    printf("array of ");
  }
  else if(str[0] == '(' && str[len-1] == ')') // parens
  {
    str[len-1] = 0;
    str++;
    dcl(str);
  }
  else if(isalpha(*str)) // name (base case)
  {
    char name[100];
    sscanf(str, "%99[a-zA-Z]", name);
    printf("%s: ", name);
  }
}

void wrapper(char *str)
{
  // skip beginning whitespace
  while(*str == ' ' && *str != 0)
    str++;

  int i;
  char type[100];
  while(isalpha(str[i]) && str[i] != 0)
  {
    type[i] = str[i];
    i++;
  }
  str += i;

  dcl(str);

  printf("%s\n", type);

}
  

int main()
{
  char str[] = "char (*(*x[])())[]";
  wrapper(str);
  
}
