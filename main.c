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

#define CHAR_MAX 255


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



enum tok_type {NOTOK, ERRTOK, KEYWORD, IDENT, STRLIT, CHAR, UNCERTAIN, INTEGER, FLOATING, SEMICOLON, PARENOP, PARENCL, BRACEOP, BRACECL, BRACKOP, BRACKCL, OPERATOR, COMMASEP, COLON, QUESTION};

enum op_type {FCALL, ARRIND, ARROW, DOT, LOGNOT, BITNOT, INC, DEC, UNPLUS, UNMIN, DEREF, CAST, SIZEOF, TIMES, DIV, MOD, BINPLUS, BINMIN, SHL, SHR, LESS, LEQ, GREAT, GEQ, EQEQ, NOTEQ, BITAND, BITXOR, BITOR, LOGAND, LOGOR, TERNARY, TERNARYQUEST, EQ, PLUSEQ, MINEQ, TIMESEQ, DIVEQ, MODEQ, ANDEQ, XOREQ, OREQ, SHLEQ, SHREQ, COMMA, PLUS, MIN, STAR};


// human readable
char *hrtok[100] = {
[NOTOK]="NOTOK", [ERRTOK]="ERRTOK", [KEYWORD]="KEYWORD", [IDENT]="IDENT", [STRLIT]="STRLIT", [CHAR]="CHAR", [UNCERTAIN]="UNCERTAIN", [INTEGER]="INTEGER", [FLOATING]="FLOATING", [SEMICOLON]="SEMICOLON", [PARENOP]="PARENOP", [PARENCL]="PARENCL", [BRACEOP]="BRACEOP", [BRACECL]="BRACECL", [BRACKOP]="BRACKOP", [BRACKCL]="BRACKCL", [OPERATOR]="OPERATOR", [COMMASEP]="COMMASEP", [COLON]="COLON", [QUESTION]="QUESTION"};

char *hrop[100] = {
  [FCALL]="FCALL",
[ARRIND]="ARRIND", [ARROW]="ARROW", [DOT]="DOT", [LOGNOT]="LOGNOT", [BITNOT]="BITNOT", [INC]="INC", [DEC]="DEC", [UNPLUS]="UNPLUS", [UNMIN]="UNMIN", [DEREF]="DEREF", [CAST]="CAST", [SIZEOF]="SIZEOF", [TIMES]="TIMES", [DIV]="DIV", [MOD]="MOD", [BINPLUS]="BINPLUS", [BINMIN]="BINMIN", [SHL]="SHL", [SHR]="SHR", [LESS]="LESS", [LEQ]="LEQ", [GREAT]="GREAT", [GEQ]="GEQ", [EQEQ]="EQEQ", [NOTEQ]="NOTEQ", [BITAND]="BITAND", [BITXOR]="BITXOR", [BITOR]="BITOR", [LOGAND]="LOGAND", [LOGOR]="LOGOR", [TERNARY]="TERNARY", [TERNARYQUEST]="TERNARYQUEST", [EQ]="EQ", [PLUSEQ]="PLUSEQ", [MINEQ]="MINEQ", [TIMESEQ]="TIMESEQ", [DIVEQ]="DIVEQ", [MODEQ]="MODEQ", [ANDEQ]="ANDEQ", [XOREQ]="XOREQ", [OREQ]="OREQ", [SHLEQ]="SHLEQ", [SHREQ]="SHREQ", [COMMA]="COMMA", [PLUS]="PLUS", [MIN]="MIN", [STAR]="STAR"};

// flags
int LONG = 1;
int UNSIGNED = 2;

int LONGDOUBLE = 1;
int FLOAT = 2;

typedef enum tok_type tok_type;
typedef enum int_len int_len;

typedef struct tok
{
  // TODO
  tok_type type; // char, string lit, cast, etc.
  // union {/* TODO */} data;
  void *data;
  int flags;

  /* modifiers (e.x. long, short, etc.) */

} tok;

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
  static char *keywords[] =
  {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while"};
  static int len = sizeof(keywords) / sizeof(char *);

  for(int i = 0; i < len; i++)
  {
    if(!strcmp(keywords[i], s))
    {
      return 1;
    }
  }
  return 0;
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
    t.type = NOTOK;
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
      str[c++] = src[i];
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
      t.type = OPERATOR;
      t.flags = SIZEOF;
      return t;
    }

    if(iskeyword(str)) // keyword
    {
      t.type = KEYWORD;
      t.data = str;
    }
    else // identifier
    {
      t.type = IDENT;
      t.data = str;
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

    assert(isdigit(src[i]) || src[i] == 'x' || src[i] == 'X'); // digit or hex indicator

    str[c++] = src[i++]; // second digit
    resize(str, c, size);

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

      t.type = INTEGER;
      t.flags = 0;

      if(isletter(src[i])) // suffix time
      {
        char s1 = tolower(src[i]);
        if(isletter(src[i+1])) // two suffixes!
        {
          char s2 = tolower(src[i+1]);
          assert((s1 == 'u' && s2 == 'l') || (s1 == 'l' && s2 == 'u')); // only combinations of two int suffixes, hardcoded to save trouble
          t.flags += LONG + UNSIGNED;

          i += 2;
        }
        else if(s1 == 'u')
        {
          t.flags = UNSIGNED;
          i++;
        }
        else if(s1 == 'l')
        {
          t.flags = LONG;
          i++;
        }
        else
          assert(!"invalid integer suffix");
      }

      // we either don't check for overflows or do so later
      u_int64_t *num = malloc(1*sizeof(u_int64_t)); // store in 64 bits regardless of actual size
      *num = 0;

      int i = 0; // for indexing string soon
      // determine base
      int base = 10;
      if(str[0] == 0 && (str[1] == 'x' || str[1] == 'X'))
      {
        base = 16;
        i += 2;
      }
      else if(str[0] == 0) // 0 on its own can also be seen as octal
        base = 8;

      // read str into num
      while(str[i])
      {
        *num += xtod(str[i]); // xtod works on octal and decimal numbers too
        *num *= base;
        i++;
      }

      free(str); // no memory leaks to be found here, maybe

      t.data = num;

      return t;
    }

    else if(src[i] == '.') // float constant
    {
leaddot:
      str[c++] = src[i++]; // write the dot
      resize(str, c, size);

      t.type = FLOATING;

      while(isxdigit(src[i])) // write the fractional part
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
      // therefore, stupidly, the sizes for floating points will depend on what compiler this compiler is compiled in
      if(suf == 'f') // float
      {
        float *num = malloc(sizeof(float)*1);
        sscanf(str, "%f", num);
        t.data = num;

        t.flags = FLOAT;
      }
      else if(suf == 'l') // long double
      {
        long double *num = malloc(sizeof(long double)*1);
        sscanf(str, "%Lf", num);
        t.data = num;

        t.flags = LONGDOUBLE;
      }
      else // double
      {
        double *num = malloc(sizeof(double)*1);
        sscanf(str, "%lf", num);
        t.data = num;

        t.flags = 0;
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
      str[c++] = src[i];
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
    
    t.type = STRLIT;
    t.data = str;

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
      str[c++] = src[i];
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

    char *chr = malloc(sizeof(char) * 1);
    *chr = str[0];
    
    t.type = CHAR;
    t.data = chr;

    return t;
  }

  else if(src[i] == ';') // semicolon, easiest case
  {
    t.type = SEMICOLON;
    i++;
    return t;
  }

  // separators
  else if(src[i] == '(') // also sometimes part of operator
  {
    t.type = PARENOP;
    i++;
    return t;
  }
  else if(src[i] == ')') // also sometimes part of operator
  {
    t.type == PARENCL;
    i++;
    return t;
  }
  else if(src[i] == '{')
  {
    t.type == BRACEOP;
    i++;
    return t;
  }
  else if(src[i] == '}')
  {
    t.type == BRACECL;
    i++;
    return t;
  }
  else if(src[i] == '[')
  {
    t.type == BRACKOP;
    i++;
    return t;
  }
  else if(src[i] == ']')
  {
    t.type == BRACKCL;
    i++;
    return t;
  }

  // now for operators
  // sometimes operators do not appear whole
  // e.x. function calls and casts
  // we mark all parens as just parens and leave it to later logic to sort out what's actually going on
  
  t.type = OPERATOR; // process of elimination
  // we use t.flags to store the operator subtype because bad practice

  // note that there are identical unary and binary operators
  // those are determined by later context
  
  if(src[i] == '-') // - -- -> -=
  {
    i++;
    if(src[i] == '>')
    {
      i++;
      t.flags = ARROW;
    }
    else if(src[i] == '-')
    {
      i++;
      t.flags = DEC;
    }
    else if(src[i] == '=')
    {
      i++;
      t.flags = MINEQ;
    }
    else
      t.flags = MIN; // MIN is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '.') // guaranteed now not to be float
  {
    i++;
    t.flags = DOT;
  }

  else if(src[i] == '!') // ! !=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.flags = NOTEQ;
    }
    else
      t.flags = LOGNOT;
  }

  else if(src[i] == '~')
  {
    i++;
    t.flags = BITNOT;
  }

  else if(src[i] == '+') // + += ++
  {
    i++;
    if(src[i] == '+')
    {
      i++;
      t.flags == INC;
    }
    else if(src[i] == '=')
    {
      i++;
      t.flags == PLUSEQ;
    }
    else
      t.flags == PLUS; // PLUS is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '*') // * *=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.flags = TIMESEQ;
    }
    else
      t.flags = STAR; // STAR is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '/') // / /*
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.flags = DIVEQ;
    }
    else
      t.flags = DIV;
  }

  else if(src[i] == '%') // % %=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.flags = MODEQ;
    }
    else
      t.flags = MOD;
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
        t.flags = SHLEQ;
      }
      else
        t.flags = SHL;
    }
    else if(src[i] == '=')
    {
      i++;
      t.flags = LEQ;
    }
    else
      t.flags = LESS;
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
        t.flags = SHREQ;
      }
      else
        t.flags = SHR;
    }
    else if(src[i] == '=')
    {
      i++;
      t.flags = GEQ;
    }
    else
      t.flags = GREAT;
  }

  else if(src[i] == '=') // = ==
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.flags = EQEQ;
    }
    else
      t.flags = EQ;
  }

  else if(src[i] == '&') // & && &=
  {
    i++;
    if(src[i] == '&')
    {
      i++;
      t.flags = LOGAND;
    }
    else if(src[i] == '=')
    {
      i++;
      t.flags = ANDEQ;
    }
    else
      t.flags = BITAND;
  }

  else if(src[i] == '|') // | || |=
  {
    i++;
    if(src[i] == '|')
    {
      i++;
      t.flags = LOGOR;
    }
    else if(src[i] == '=')
    {
      i++;
      t.flags = OREQ;
    }
    else
      t.flags = BITOR;
  }

  else if(src[i] == '^')
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t.flags = XOREQ;
    }
    else
      t.flags = BITXOR;
  }

  else if(src[i] == ',') // can be separator or operator. assume separator for now
  {
    t.type = COMMASEP;
  }

  else if(src[i] == ':') // can be part of label or part of ternary
  {
    t.type = COLON;
  }

  else if(src[i] == '?') // part of ternary (incomplete, must complete later)
  {
    t.flags = TERNARYQUEST;
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
  int x;
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
