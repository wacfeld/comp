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
#include <stdlib.h>
#include <string.h>
// #include <time.h>
#define putd(x) printf(#x ": %d\n", x)
#define newl() puts("")


#define allocstr(str, size, c) int size = 10; int c = 0; char *str = malloc(size);

#define CHAR_MAX 255
#define CHAR_SIZE 1
#define FLOAT_SIZE 4
#define INT_SIZE 4

// void decomment(char *src)
// {
//   int len = strlen(src);
//   // don't nest
//   // ignore in string literals
//   // ignore in character literals
//   // must terminate before EOF
  
  
// }


// replaces backslash + newline with nothing
void splice(char *src)
{
  /* int len = strlen(src); */
  int c = 0, i = 0;
  while(src[i])
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
  /* int len = strlen(src); */
  int c = 0;
  int i = 0;
  while(src[i])
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

// mark backslash escape sequences
void mark_esc(char *src, char *esc)
{
  int i;
  /* int len = strlen(src); */
  for(i = 0; src[i]; i++) // clear esc
  {
    esc[i] = 0;
  }

  /*
    following splice(), the only backslashes must be in char/string literals
    we can mark these, so that we know which single/double quotes to ignore later
    then once we have determined string/char literals, we can search again for stray backslashes
  */
  i = 0;
  while(src[i])
  {
    if(src[i] == '\\')
    {
      esc[i] = 1;
      esc[i+1] = 1;
      // the thing afterward is skipped, as it might be a backslash again
      i += 2;
    }
    else
      i++;
  }
}


// it is important to mark string/char literals first so that we know what not to take literally
void mark_quot(char *src, char *esc, char *quot)
{
  int i;
  /* int len = strlen(src); */
  for(i = 0; src[i]; i++) // clear quot
  {
    quot[i] = 0;
  }

  char curquot = 0; // either 0, single quote, or double quote
  for(i = 0; src[i]; i++)
  {
    quot[i] = curquot;

    if((src[i] == '"' || src[i] == '\'') && !esc[i]) // non-escaped quote
    {
      if(curquot == 0) // entering string
      {
        curquot = src[i];
      }
      else if(curquot == src[i]) // same quote type, exiting string
      {
        curquot = 0;
        quot[i] = 0;
      }
      // else different quote type, ignore
    }
  }
}


void stray_backslash(char *src, char *esc, char *quot)
{
  for(int i = 0; src[i]; i++)
  {
    assert(src[i] != '\\' || quot[i]);
  }
}


void rem_comments(char *src, char *esc, char *quot)
{
  int i = 0, c = 0;
  int incomment = 0;
  while(src[i])
  {
    if(src[i] == '/' && src[i+1] == '*' && !quot[i] && !quot[i+1]) // comment not in quotations
    {
      incomment = 1;
      i += 2; // avoid /*/ occurrences which are not comments
    }
    else if(incomment && src[i] == '*' && src[i+1] == '/') // comment close
    {
      incomment = 0;
      src[c] = ' '; // replace whole comment with space
      esc[c] = 0;
      quot[c] = 0;
      i += 2;
      c++;
    }

    else if(!incomment) // not in comment, copy over
    {
      src[c] = src[i];
      quot[c] = quot[i];
      esc[c] = esc[i];
      c++;
      i++;
    }
    else // don't copy anything
      i++;
  }

  assert(!incomment); // no unterminated comments
  src[c] = 0; // null terminate
  
}


// int **toplevel(char *src, char *esc, char *quot)
// {
//   int size = 100;
//   int count = 0;
//   int (*statements)[2] = malloc(size*sizeof(int [2]));
//   int start = 0, end; // statement start and end
//   int paren_dep = 0, brace_dep = 0; // depth of parens and braces

//   int i;
//   for(i = 0; src[i]; i++)
//   {
//     // update depths of parens/braces
//     if(src[i] == '{')
//       brace_dep++;
//     if(src[i] == '}')
//       brace_dep--;
//     if(src[i] == '(')
//       paren_dep++;
//     if(src[i] == ')')
//       paren_dep--;
    
//     if((src[i] == ';' && !paren_dep && !brace_dep)) // end of top level statement
//     {
//       end = i+1;
//       statements[count][0] = start;
//       statements[count][1] = end;

//       start = i + 1;

//       count++;
//     }

//     else if(src[i] == '}' && !paren_dep && !brace_dep) // possibly end of top level compound statement
//     {
//       end = i+1;
//       statements[count][0] = start;
//       statements[count][1] = end;

//       start = i+1;
//     }
//   }
// }



enum tok_type {NOTOK, ERRTOK, KEYWORD, IDENT, STRLIT, CHAR, UNCERTAIN, INTEGER, FLOATING, ATOM};

enum atom_type {FCALL, ARRIND, ARROW, DOT, LOGNOT, BITNOT, INC, DEC, UNPLUS, UNMIN, DEREF, CAST, SIZEOF, TIMES, DIV, MOD, BINPLUS, BINMIN, SHL, SHR, LESS, LEQ, GREAT, GEQ, EQEQ, NOTEQ, BITAND, BITXOR, BITOR, LOGAND, LOGOR, TERNARY, TERNARYQUEST, EQ, PLUSEQ, MINEQ, TIMESEQ, DIVEQ, MODEQ, ANDEQ, XOREQ, OREQ, SHLEQ, SHREQ, COMMA, PLUS, MIN, STAR, COLON, QUESTION, SEMICOLON, PARENOP, PARENCL, BRACEOP, BRACECL, BRACKOP, BRACKCL};

typedef enum tok_type tok_type;
typedef enum int_len int_len;

// replaced by union of structs
// typedef struct tok
// {
//   // TODO
//   tok_type type; // char, string lit, cast, etc.
//   // union {/* TODO */} data;
//   void *data;
//   int flags;

//   /* modifiers (e.x. long, short, etc.) */

// } tok;

typedef union
{
  struct 
  {
    tok_type type;
  } gen;

  struct
  {
    tok_type type;
    int cont;
  } keyword;

  struct
  {
    tok_type type;
    char *cont;
  } ident;

  struct
  {
    tok_type type;
    char *cont;
  } strlit;

  struct
  {
    tok_type type;
    char cont;
  } character; // char is reserved

  struct
  {
    tok_type type;
    u_int32_t cont;
    int islong;
    int isunsigned;
  } integer;

  struct
  {
    tok_type type;
    float cont;
    int islong;
    int isshort;
  } floating;

  struct
  {
    tok_type type;
    enum atom_type cont;
  } atom;

} tok;

// human readable, for debugging purposes only
char *hrtok[100] = {
[NOTOK]="NOTOK", [ERRTOK]="ERRTOK", [KEYWORD]="KEYWORD", [IDENT]="IDENT", [STRLIT]="STRLIT", [CHAR]="CHAR", [UNCERTAIN]="UNCERTAIN", [INTEGER]="INTEGER", [FLOATING]="FLOATING", [SEMICOLON]="SEMICOLON", [PARENOP]="PARENOP", [PARENCL]="PARENCL", [BRACEOP]="BRACEOP", [BRACECL]="BRACECL", [BRACKOP]="BRACKOP", [BRACKCL]="BRACKCL", [ATOM]="ATOM"};

char *hrat[100] = {
  [FCALL]="FCALL",
[ARRIND]="ARRIND",
[ARROW]="ARROW",
[DOT]="DOT",
[LOGNOT]="LOGNOT",
[BITNOT]="BITNOT",
[INC]="INC",
[DEC]="DEC",
[UNPLUS]="UNPLUS",
[UNMIN]="UNMIN",
[DEREF]="DEREF",
[CAST]="CAST",
[SIZEOF]="SIZEOF",
[TIMES]="TIMES",
[DIV]="DIV",
[MOD]="MOD",
[BINPLUS]="BINPLUS",
[BINMIN]="BINMIN",
[SHL]="SHL",
[SHR]="SHR",
[LESS]="LESS",
[LEQ]="LEQ",
[GREAT]="GREAT",
[GEQ]="GEQ",
[EQEQ]="EQEQ",
[NOTEQ]="NOTEQ",
[BITAND]="BITAND",
[BITXOR]="BITXOR",
[BITOR]="BITOR",
[LOGAND]="LOGAND",
[LOGOR]="LOGOR",
[TERNARY]="TERNARY",
[QUESTION]="QUESTION",
[EQ]="EQ",
[PLUSEQ]="PLUSEQ",
[MINEQ]="MINEQ",
[TIMESEQ]="TIMESEQ",
[DIVEQ]="DIVEQ",
[MODEQ]="MODEQ",
[ANDEQ]="ANDEQ",
[XOREQ]="XOREQ",
[OREQ]="OREQ",
[SHLEQ]="SHLEQ",
[SHREQ]="SHREQ",
[COMMA]="COMMA",
[COLON]="COLON",
[PLUS]="PLUS",
[MIN]="MIN",
[STAR]="STAR",
[SEMICOLON]="SEMICOLON",
[PARENOP]="PARENOP",
[PARENCL]="PARENCL",
[BRACEOP]="BRACEOP",
[BRACECL]="BRACECL",
[BRACKOP]="BRACKOP",
[BRACKCL]="BRACKCL",
};

enum keyword {K_AUTO, K_BREAK, K_CASE, K_CHAR, K_CONST, K_CONTINUE, K_DEFAULT, K_DO, K_DOUBLE, K_ELSE, K_ENUM, K_EXTERN, K_FLOAT, K_FOR, K_GOTO, K_IF, K_INT, K_LONG, K_REGISTER, K_RETURN, K_SHORT, K_SIGNED, K_STATIC, K_STRUCT, K_SWITCH, K_TYPEDEF, K_UNION, K_UNSIGNED, K_VOID, K_VOLATILE, K_WHILE}; // sizeof not here because it's an operator
char *keywords[] =
{
  [K_AUTO]="auto",
  [K_BREAK]="break",
  [K_CASE]="case",
  [K_CHAR]="char",
  [K_CONST]="const",
  [K_CONTINUE]="continue",
  [K_DEFAULT]="default",
  [K_DO]="do",
  [K_DOUBLE]="double",
  [K_ELSE]="else",
  [K_ENUM]="enum",
  [K_EXTERN]="extern",
  [K_FLOAT]="float",
  [K_FOR]="for",
  [K_GOTO]="goto",
  [K_IF]="if",
  [K_INT]="int",
  [K_LONG]="long",
  [K_REGISTER]="register",
  [K_RETURN]="return",
  [K_SHORT]="short",
  [K_SIGNED]="signed",
  [K_STATIC]="static",
  [K_STRUCT]="struct",
  [K_SWITCH]="switch",
  [K_TYPEDEF]="typedef",
  [K_UNION]="union",
  [K_UNSIGNED]="unsigned",
  [K_VOID]="void",
  [K_VOLATILE]="volatile",
  [K_WHILE]="while"};


void puttok(tok t)
{
  printf("%s ", hrtok[t.gen.type]);
  if(t.gen.type == ATOM)
  {
    printf("%s ", hrat[t.atom.cont]);
  }
  if(t.gen.type == FLOATING)
    printf("%f ", t.floating.cont);
  if(t.gen.type == STRLIT || t.gen.type == IDENT)
    printf("%s ", t.strlit.cont);
  if(t.gen.type == KEYWORD)
  {
    printf("%s ", keywords[t.keyword.cont]);
  }
  if(t.gen.type == CHAR)
    printf("%c ", t.character.cont);
  if(t.gen.type == INTEGER)
  {
    printf("%d ", t.integer.cont);
  }
  newl();
}

// flags (no longer in use, replaced by individual variables
// int LONG = 1;
// int UNSIGNED = 2;

// int LONGDOUBLE = 1;
// int FLOAT = 2;


// gets next (toplevel) statement from string & tokenizes
// these two steps must be done simultaneously, as far as i can tell
// otherwise it's very difficult to tell when a statement ends
tok *nextstat(char *src, char *esc, char *quot)
{
}


int isletter(char c)
{
  return (isalpha(c) || c == '_'); // identifier letters can be [a-zA-Z] or underscore
}

int iskeyword(char *s)
{
  static int len = sizeof(keywords) / sizeof(char *);

  for(int i = 0; i < len; i++)
  {
    if(!strcmp(keywords[i], s))
    {
      return i;
    }
  }
  return -1;
}


// int isoperator(char c)
// {
//   static char operators[] = "!%&*+
  
// }


int isintsuffix(char c)
{
  return c == 'l' || c == 'L' || c == 'u' || c == 'U';
}

int isfloatsuffix(char c)
{
  return c == 'l' || c == 'L' || c == 'f' || c == 'F';
}

// use realloc when necessary to expand a dynamically allocated array
#define resize(p, c, s) if(c >= s) {s *= 2; p = realloc(p, s);}

tok nexttok(char *src, char *esc, char *quot)
{
  // lots of this might be doable with regex, but i'm not sure

  static int i = 0;

  tok t;
  if(src == NULL) // reset i
  {
    i = 0;
    // doesn't actually matter what we return
    return t;
  }

  while(isspace(src[i])) i++; // skip leading whitespace

  if(!src[i]) // end of src
  {
    t.gen.type = NOTOK;
    return t; // signal no token to caller
  }
  
  int size = 10;
  int c = 0;
  char *str = malloc(size * sizeof(char));

  if(src[i] == '.' && isdigit(src[i+1])) // leading dot floating point
  {
    goto leaddot; // here be raptors
  }
  else if(isletter(src[i])) // identifier or keyword or enum constant (or sizeof)
  {
    // int size = 10;
    // int c = 0;
    // char *str = malloc(size * sizeof(char));

    // get the certain tokens out of the way

    // write identifier/keyword into str
    while(isletter(src[i]) || isdigit(src[i]))
    {
      str[c++] = src[i++];
      // if(c >= size)
      // {
      //   str = realloc(str, size*2);
      //   size *= 2;
      // }
      resize(str, c, size);

    }
    str[c] = 0;

    if(!strcmp(str, "sizeof")) // special case, sizeof is an operator
    {
      t.gen.type = ATOM;
      t.atom.cont = SIZEOF;
      return t;
    }

    int k;
    if((k = iskeyword(str)) != -1) // keyword
    {
      t.gen.type = KEYWORD;
      t.keyword.cont = k;
    }
    else // identifier
    {
      t.gen.type = IDENT;
      t.ident.cont = str;
    }

    return t;
  }

  else if(isdigit(src[i])) // integer or float constant
  {
    // int size = 10;
    // char *str = malloc(size * sizeof(char));
    // int c = 0;

    str[c++] = src[i++]; // first digit
    resize(str, c, size);

    // assert(isdigit(src[i]) || src[i] == 'x' || src[i] == 'X'); // digit or hex indicator

    if(isdigit(src[i]) || src[i] == 'x' || src[i] == 'X') // digit or hex indicator
    {
      str[c++] = src[i++]; // second digit
      resize(str, c, size);
    }

    while(isxdigit(src[i])) // rest of digits
    {
      str[c++] = src[i++];
      // if(c >= size)
      // {
      //   str = realloc(str, size*2);
      //   size *= 2;
      // }
      resize(str, c, size);
    }

    /*
operators/delimiters:
      !%&()*+,-./:;<=>?[]^{|}~
quotes:
      "'
banned:
      #$@\`
digits:
      0123456789
letters:
      ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_
whitespace:

       */

    // see above list for process-of-elimination reasoning below
    // if(isintsuffix(src[i]) || !isletter(src[i])) // integer
    if(src[i] != '.') // no decimal point, therefore integer
    {
      str[c] = 0; // null terminate

      t.gen.type = INTEGER;
      t.integer.isunsigned = 0;
      t.integer.islong = 0;

      if(isletter(src[i])) // suffix time
      {
        char s1 = tolower(src[i]);
        if(isletter(src[i+1])) // two suffixes!
        {
          char s2 = tolower(src[i+1]);
          assert((s1 == 'u' && s2 == 'l') || (s1 == 'l' && s2 == 'u')); // only combinations of two int suffixes, hardcoded to save trouble
          t.integer.isunsigned = 1;
          t.integer.islong = 1;

          i += 2;
        }
        else if(s1 == 'u')
        {
          t.integer.isunsigned = 1;
          i++;
        }
        else if(s1 == 'l')
        {
          t.integer.islong = 1;
          i++;
        }
        else
          assert(!"invalid integer suffix");
      }

      // we either don't check for overflows or do so later
      u_int32_t num = 0;

      assert(tolower(str[1]) != 'x' || str[2] != 0);
      int i = 0; // for indexing string soon
      // determine base
      int base = 10;
      if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
      {
        base = 16;
        i += 2;
      }
      else if(str[0] == '0') // 0 on its own can also be seen as octal
        base = 8;

      // read str into num
      while(str[i])
      {
        num *= base;
        num += xtod(str[i]); // xtod works on octal and decimal numbers too
        i++;
      }

      free(str); // no memory leaks to be found here, maybe

      t.integer.cont = num;

      return t;
    }

    else if(src[i] == '.') // float constant
    {
leaddot:
      str[c++] = src[i++]; // write the dot
      resize(str, c, size);

      t.gen.type = FLOATING;
      t.floating.isshort = 0;
      t.floating.islong = 0;

      while(isdigit(src[i])) // write the fractional part
      {
        str[c++] = src[i++];
        resize(str, c, size);
      }

      if(src[i] == 'e' || src[i] == 'E') // exponent
      {
        str[c++] = src[i++]; // write exponent
        resize(str, c, size);

        if(src[i] == '+' || src[i] == '-') // sign
        {
          str[c++] = src[i++]; // write sign
          resize(str, c, size);
        }

        assert(isdigit(src[i])); // digits must follow, regardless of sign
        while(isdigit(src[i])) // write digits
        {
          str[c++] = src[i++];
          resize(str, c, size);
        }
      }

      str[c] = 0; // null terminate
      
      char suf = 0;
      if(isfloatsuffix(src[i]))
      {
        // parse suffix
        suf = tolower(src[i]);
        i++;
      }
      else assert(!isletter(src[i])); // if no suffix, must not be letter

      // get c stdlib to do this for me because i'm lazy
      // all floating points are the same size, for convenience (the standard allows this)

      float num;
      if(suf == 'f') // float
      {
        sscanf(str, "%f", &num);
        t.floating.cont = num;

        t.floating.isshort = 1;
      }
      else if(suf == 'l') // long double
      {
        sscanf(str, "%f", &num);
        t.floating.cont = num;

        t.floating.islong = 1;
      }
      else // double
      {
        sscanf(str, "%f", &num);
        t.floating.cont = num;
      }

      free(str); // no longer needed
      
      return t;
    }

    assert(!"this assert should never run");
  }

  else if(src[i] == '"') // string literal
  {
    i++; // skip quotation mark

    // int size = 10;
    // char *str = malloc(size * sizeof(char));
    // int c = 0;

    // write string into str
    while(quot[i])
    {
      str[c++] = src[i++];
      resize(str, c, size);

      if(c > size)
      {
        str = realloc(str, size*2);
        size *= 2;
      }
    }
    str[c] = 0;
    i++; // skip closing quotation mark

    unesc(str); // convert escape sequences into the real deal
    
    t.gen.type = STRLIT;
    t.strlit.cont = str;

    return t;
  }

  else if(src[i] == '\'') // character constant
  {
    i++; // skip quote

    // int size = 10;
    // char *str = malloc(size * sizeof(char));
    // int c = 0;

    // write string into str
    while(quot[i])
    {
      str[c++] = src[i++];
      resize(str, c, size);

      if(c > size)
      {
        str = realloc(str, size*2);
        size *= 2;
      }
    }
    str[c] = 0;
    i++; // skip closing quotation mark

    unesc(str); // convert escape sequences into the real deal
    assert(strlen(str) == 1); // the final literalized character should be unaccompanied

    char chr;
    chr = str[0];
    
    t.gen.type = CHAR;
    t.character.cont = chr;

    return t;
  }

  else if(src[i] == ';') // semicolon, easiest case
  {
    t.gen.type = ATOM;
    t.atom.cont = SEMICOLON;
    i++;
    return t;
  }

  // separators
  else if(src[i] == '(') // also sometimes part of operator
  {
    t.gen.type = ATOM;
    t.atom.cont = PARENOP;
    i++;
    return t;
  }
  else if(src[i] == ')') // also sometimes part of operator
  {
    t.gen.type = ATOM;
      t.atom.cont = PARENCL;
    i++;
    return t;
  }
  else if(src[i] == '{')
  {
    t.gen.type = ATOM;
    t.atom.cont = BRACEOP;
    i++;
    return t;
  }
  else if(src[i] == '}')
  {
    t.gen.type = ATOM;
    t.atom.cont = BRACECL;
    i++;
    return t;
  }
  else if(src[i] == '[')
  {
    t.gen.type = ATOM;
    t.atom.cont = BRACKOP;
    i++;
    return t;
  }
  else if(src[i] == ']')
  {
    t.gen.type = ATOM;
    t.atom.cont = BRACKCL;
    i++;
    return t;
  }

  // now for operators
  // sometimes operators do not appear whole
  // e.x. function calls and casts
  // we mark all parens as just parens and leave it to later logic to sort out what's actually going on
  
  t.gen.type = ATOM; // process of elimination

  // note that there are identical unary and binary operators
  // those are determined by later context
  
  if(src[i] == '-') // - -- -> -=
  {
    i++;
    if(src[i] == '>')
    {
      i++;
      t.atom.cont = ARROW;
    }
    else if(src[i] == '-')
    {
      i++;
      t.atom.cont = DEC;
    }
    else if(src[i] == '=')
    {
      i++;
      t.atom.cont = MINEQ;
    }
    else
      t.atom.cont = MIN; // MIN is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '.') // guaranteed now not to be float
  {
    i++;
    t.atom.cont = DOT;
  }

  else if(src[i] == '!') // ! !=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.atom.cont = NOTEQ;
    }
    else
      t.atom.cont = LOGNOT;
  }

  else if(src[i] == '~')
  {
    i++;
    t.atom.cont = BITNOT;
  }

  else if(src[i] == '+') // + += ++
  {
    i++;
    if(src[i] == '+')
    {
      i++;
      t.atom.cont = INC;
    }
    else if(src[i] == '=')
    {
      i++;
      t.atom.cont = PLUSEQ;
    }
    else
      t.atom.cont = PLUS; // PLUS is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '*') // * *=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.atom.cont = TIMESEQ;
    }
    else
      t.atom.cont = STAR; // STAR is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '/') // / /*
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.atom.cont = DIVEQ;
    }
    else
      t.atom.cont = DIV;
  }

  else if(src[i] == '%') // % %=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.atom.cont = MODEQ;
    }
    else
      t.atom.cont = MOD;
  }

  else if(src[i] == '<') // < << <<= <=
  {
    i++;
    if(src[i] == '<')
    {
      i++;
      if(src[i] == '=')
      {
        i++;
        t.atom.cont = SHLEQ;
      }
      else
        t.atom.cont = SHL;
    }
    else if(src[i] == '=')
    {
      i++;
      t.atom.cont = LEQ;
    }
    else
      t.atom.cont = LESS;
  }

  else if(src[i] == '>') // > >> >>= >=
  {
    i++;
    if(src[i] == '>')
    {
      i++;
      if(src[i] == '=')
      {
        i++;
        t.atom.cont = SHREQ;
      }
      else
        t.atom.cont = SHR;
    }
    else if(src[i] == '=')
    {
      i++;
      t.atom.cont = GEQ;
    }
    else
      t.atom.cont = GREAT;
  }

  else if(src[i] == '=') // = ==
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.atom.cont = EQEQ;
    }
    else
      t.atom.cont = EQ;
  }

  else if(src[i] == '&') // & && &=
  {
    i++;
    if(src[i] == '&')
    {
      i++;
      t.atom.cont = LOGAND;
    }
    else if(src[i] == '=')
    {
      i++;
      t.atom.cont = ANDEQ;
    }
    else
      t.atom.cont = BITAND;
  }

  else if(src[i] == '|') // | || |=
  {
    i++;
    if(src[i] == '|')
    {
      i++;
      t.atom.cont = LOGOR;
    }
    else if(src[i] == '=')
    {
      i++;
      t.atom.cont = OREQ;
    }
    else
      t.atom.cont = BITOR;
  }

  else if(src[i] == '^')
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.atom.cont = XOREQ;
    }
    else
      t.atom.cont = BITXOR;
  }

  else if(src[i] == ',') // can be separator or operator. assume separator for now
  {
    i++;
    t.atom.cont = COMMA;
  }

  else if(src[i] == ':') // can be part of label or part of ternary
  {
    i++;
    t.atom.cont = COLON;
  }

  else if(src[i] == '?') // part of ternary (incomplete, must complete later)
  {
    i++;
    t.atom.cont = QUESTION;
  }
  
  return t;
  /*
  floats can start with dots
  colons can be for labels
  parens/braces can be many things
  ternary operators
  minus sign can be arrow operator
     */


}

void check_stray(char *src, char *esc, char *quot, char *banned)
{
  int i;
  for(i = 0; src[i]; i++)
  {
    assert(strchr(banned, src[i]) == NULL || quot[i]);
  }
}

int main()
{
  // int x;
  // (x) = 5;
  // putd(x);
  // int hellonumber = 5UL;
  // int $hello = 5;
  // putd($hello);
  // float x = 1.;
  // printf("%f\n", x);
  // int k = 0;
  // for(int k = 0; k < 256; k++)
  // {
  //   if(isprint(k) && !isspace(k))
  //     putchar(k);
  // }
  // newl();
  // float f = -0.5;
  // printf("%f\n", f);
  
  assert(sizeof(float) == 4); // there is no int32_t analog for floats

  int c;
  int i = 0;
  char src[1000], quot[1000], esc[1000];

  while((c = getchar()) != EOF) // read in src
  {
    src[i++] = c;
  }
  src[i] = 0; // null terminate

  splice(src); // delete backslash + newline combinations

  mark_esc(src, esc); // mark backslash escape sequences
  mark_quot(src, esc, quot); // mark single and double quoted regions

  rem_comments(src, esc, quot); // replace multiline comments with single space

  // stray_backslash(src, esc, quot); // check for stray backslashes, throw a tantrum if so
  check_stray(src, esc, quot, "#$@\\`"); // check for stray characters, throw a tantrum if so

  tok t;
  while((t = nexttok(src, esc, quot)).gen.type != NOTOK)
    puttok(t);


  // get all top level statements

  // printf("%s", src);
  // for(i = 0; i < strlen(src); i++)
  // {
  //   if(esc[i]) putchar('e');
  //   else putchar('.');
  // }
  // puts("");
  // for(i = 0; i < strlen(src); i++)
  // {
  //   if(quot[i] == 0)
  //   {
  //     putchar('.');
  //   }
  //   else
  //     putchar(quot[i]);
  // }

}
