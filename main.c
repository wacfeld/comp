#include <stdio.h>
#include <assert.h>
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

#define CHAR_MAX 255


void decomment(char *src)
{
  int len = strlen(src);
  // don't nest
  // ignore in string literals
  // ignore in character literals
  // must terminate before EOF
  
  
}


// replaces backslash + newline with nothing
void fold(char *src)
{
  int len = strlen(src);
  int c = 0, i = 0;
  while(i < len)
  {
    if(src[i] == '\\' && src[i+1] == '\n')
    {
      i += 2;
      
    }
    else
    {
      src[c++] = src[i];
      i++;
    }
  }
  src[c] = 0;
}

// need to figure out what is a string taking into account \" and \'


// already exists
/* int ishexdigit(char c) */
/* { */
/*   return isdigit(c) || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f'); */
/* } */

int xtod(char c)
{
  if(isdigit(c))
    return c - '0';

  return tolower(c) - 'a' + 10;
}

// literalizes escape sequences
void unesc(char *src)
{
  int len = strlen(src);
  int c = 0;
  int i = 0;
  while(i < len)
  {
    if(src[i] == '\\')
    {
      if(src[i+1] == '\\')
      {
        src[c++] = '\\';
        i += 2;
      }
      else if(src[i+1] == 'n')
      {
        src[c++] = '\n';
        i += 2;
      }
      else if(src[i+1] == 't')
      {
        src[c++] = '\t';
        i += 2;
      }
      else if(src[i+1] == 'v')
      {
        src[c++] = '\v';
        i += 2;
      }
      else if(src[i+1] == 'b')
      {
        src[c++] = '\b';
        i += 2;
      }
      else if(src[i+1] == 'r')
      {
        src[c++] = '\r';
        i += 2;
      }
      else if(src[i+1] == 'f')
      {
        src[c++] = '\f';
        i += 2;
      }
      else if(src[i+1] == 'a')
      {
        src[c++] = '\a';
        i += 2;
      }
      else if(src[i+1] == '?')
      {
        src[c++] = '\t';
        i += 2;
      }
      else if(src[i+1] == '\'')
      {
        src[c++] = '\'';
        i += 2;
      }
      else if(src[i+1] == '"')
      {
        src[c++] = '"';
        i += 2;
      }
      else if(isdigit(src[i+1])) // octal
      {
        int numlen = 1;
        int num = 0;
        while(isdigit(src[i+numlen]) && numlen <= 3)
        {
          num *= 8;
          num += src[i+numlen] - '0';
          numlen++;
        }

        assert(num <= CHAR_MAX);
        src[c++] = num;
        i += numlen;
      }
      else if(src[i+1] == 'x') // hex
      {
        int numlen = 1;
        int num = 0;
        while(isxdigit(src[i+numlen+1]))
        {
          num *= 16;
          num += xtod(src[i+numlen+1]);
          numlen++;
        }

        assert(num <= CHAR_MAX);
        src[c++] = num;
        i += numlen+1;
      }
      else
      {
        assert(!"invalid escape sequence");
      }
    }
    else
    {
      src[c++] = src[i];
      i++;
    }
  }

  src[c] = 0;
}

// it is important to mark string literals first so that we know what not to take literally
void mark_strlit(char *src, char *marks)
{
  int i;
  int len = strlen(src);
  for(i = 0; i < len; i++) // clear marks
  {
    marks[i] = 0;
  }

  int instr = 0;
  for(i = 0; i < len; i++)
  {
    if(src[i] == '"')
    {
      
    }
  }
}

int main()
{
  int c;
  int i = 0;
  char src[1000];
  while((c = getchar()) != EOF)
  {
    src[i++] = c;
  }

  fold(src);
  /* unesc(src); */
  printf("%s", src);

}
