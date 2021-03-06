//{{{1 overhead
#include "defs.h"
#include "datastruct.h"
#include <execinfo.h>

// list *dam = makelist(sizeof(void *)); // dynamically allocated memory, to be freed all at once later

// attach two links, accounting for null, creating temporary variables to account for yknow
#define attach(a, b) {link *ta = a; link *tb = b; if(ta) ta->right = tb; if(tb) tb->left = ta;}

// move to either end of a linked list
// #define rightend(l) {testerr(l, "rightend: l null"); while(l->right != NULL) l = l->right;}
#define rightend(l) {assert(l); while(l->right != NULL) l = l->right;}
// #define leftend(l) {testerr(l, "leftend: l null"); while(l->left != NULL) l = l->left;}
#define leftend(l) {assert(l); while(l->left != NULL) l = l->left;}
// it's not enough to just save either end of the list because the pointer might change

// list *errors; // list of error messages
char *error;

// for non-fatal errors, which may occur when parsing expressions
// e.x. we misinterpret a unary * as binary *, and have to try again
// e.x. sizeof * a
#define testerr(e, msg) {if(!(e)) {error = msg; return NULL;}}


// the ltr bins need to check if parseltrbinexpr just went down() or found an op of its type. also it needs to check if null because we want to operate
#define checkours(e, et) {if(!(e)) return NULL; if(!eistype((e), (et))) return (e);}

void puttok(token t);


#define RIGHT 1
#define LEFT 0

int isdeclspec(token t);

//{{{1 debugging

#define throw(msg) {fprintf(stderr, "%s %d: %s\n", __func__, __LINE__, msg);/*trace()*/;exit(1);}

void trace()
{
  void* callstack[128];
  int i, frames = backtrace(callstack, 128);
  char** strs = backtrace_symbols(callstack, frames);
  puts("===========================");
  for (i = 0; i < frames; ++i) {
    printf("%s\n", strs[i]);
  }
  puts("===========================");
  free(strs);
}

//{{{1 global mutables
// variables which indicate the state of the parser, to provide context for different parsing modules

// stack of all active declarations in lexical scope, with separators to allow masking
// gets initialized in proctoplevel
stack *scope = NULL;

stack *contstack = NULL;
stack *breakstack = NULL;

// 1 when we just entered a fundef's block statement, to indicate that a separator should not be created
// int startfundef = 0;
ctype funret = NULL;
int framesize = 0;

int literals_len = 100;
char *literals = NULL;


//{{{1 lexer

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
        throw("invalid escape sequence");
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

void putll(link *l) // put linked list
{
  while(l != NULL)
  {
    if(l->type == TOK_L)
    {
      puttok(*l->cont.tok);
    }

    if(l->type == EXPR_L)
    {
      
      // puts(hr_expr[l->cont.exp->type]);
    }

    l = l->right;
  }
}

void puttok(token t)
{
  if(t.gen.type == NOTOK)
  {
    printf("%s ", hrtok[t.gen.type]);
  }
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
  // nline();
}


// // gets next (toplevel) statement from string & tokenizes
// // these two steps must be done simultaneously, as far as i can tell
// // otherwise it's very difficult to tell when a statement ends
// token *nextstat(char *src, char *esc, char *quot)
// {
// }


int isletter(char c)
{
  return (isalpha(c) || c == '_'); // identifier letters can be [a-zA-Z] or underscore
}


int striskeyword(char *s)
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


int isintsuffix(char c)
{
  return c == 'l' || c == 'L' || c == 'u' || c == 'U';
}

int isfloatsuffix(char c)
{
  return c == 'l' || c == 'L' || c == 'f' || c == 'F';
}


token * nexttok(char *src, char *esc, char *quot)
{
  // lots of this might be doable with regex, but i'm not sure

  token *t = malloc(sizeof(token));
  static int i = 0;

  if(src == NULL) // reset i
  {
    i = 0;
    // doesn't actually matter what we return
    return t;
  }

  while(isspace(src[i])) i++; // skip leading whitespace

  if(!src[i]) // end of src
  {
    t->gen.type = NOTOK;
    return t; // signal no token to caller
  }
  
  alloc(char, str, size, c);

  if(src[i] == '.' && isdigit(src[i+1])) // leading dot floating point
  {
    goto leaddot; // here be raptors
  }
  else if(isletter(src[i])) // identifier or keyword or enum constant (or sizeof)
  {

    // get the certain tokens out of the way

    // write identifier/keyword into str
    while(isletter(src[i]) || isdigit(src[i]))
    {
      str[c++] = src[i++];
      resize(str, size, c);

    }
    str[c] = 0;

    if(!strcmp(str, "sizeof")) // special case, sizeof is an operator
    {
      t->gen.type = ATOM;
      t->atom.cont = SIZEOF;
      return t;
    }

    int k;
    if((k = striskeyword(str)) != -1) // keyword
    {
      t->gen.type = KEYWORD;
      t->keyword.cont = k;
    }
    else // identifier
    {
      t->gen.type = IDENT;
      t->ident.cont = str;
    }

    return t;
  }

  else if(isdigit(src[i])) // integer or float constant
  {
    str[c++] = src[i++]; // first digit
    resize(str, size, c);

    // assert(isdigit(src[i]) || src[i] == 'x' || src[i] == 'X'); // digit or hex indicator

    if(isdigit(src[i]) || src[i] == 'x' || src[i] == 'X') // digit or hex indicator
    {
      str[c++] = src[i++]; // second digit
      resize(str, size, c);
    }

    while(isxdigit(src[i])) // rest of digits
    {
      str[c++] = src[i++];
      resize(str, size, c);
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

      t->gen.type = INTEGER;
      t->integer.isunsigned = 0;
      t->integer.islong = 0;

      if(isletter(src[i])) // suffix time
      {
        char s1 = tolower(src[i]);
        if(isletter(src[i+1])) // two suffixes!
        {
          char s2 = tolower(src[i+1]);
          assert((s1 == 'u' && s2 == 'l') || (s1 == 'l' && s2 == 'u')); // only combinations of two int suffixes, hardcoded to save trouble
          t->integer.isunsigned = 1;
          t->integer.islong = 1;

          i += 2;
        }
        else if(s1 == 'u')
        {
          t->integer.isunsigned = 1;
          i++;
        }
        else if(s1 == 'l')
        {
          t->integer.islong = 1;
          i++;
        }
        else
          throw("invalid integer suffix");
      }

      // we either don't check for overflows or do so later
      dword num = 0;

      // assert(tolower(str[1]) != 'x' || str[2] != 0); // if there's an x, then the string can't just terminate right after the x

      if(tolower(str[1]) == 'x')
        assert(str[2] != 0); // no empty hex strings
      if(tolower(str[1]) == 'x') // 'x' implies 0 before it
        assert(str[0] == '0');

      // figure out if decimal or not, important for determining type later
      if(str[0] == '0' && tolower(str[1]) == 'x') // hex
      {
        t->integer.isdecimal = 0;
      }
      else if(str[0] == '0' && c >= 2) // octal
      {
        t->integer.isdecimal = 0;
      }
      else
        t->integer.isdecimal = 1;

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

      t->integer.cont = num;
      // assigning any smaller type to dword (u_int32_t) automatically does sign extension

      return t;
    }

    else if(src[i] == '.') // float constant
    {
      puts("Error: floating point not supported yet");
      exit(1);

leaddot:
      str[c++] = src[i++]; // write the dot
      resize(str, size, c);

      t->gen.type = FLOATING;
      t->floating.isshort = 0;
      t->floating.islong = 0;

      while(isdigit(src[i])) // write the fractional part
      {
        str[c++] = src[i++];
        resize(str, size, c);
      }

      if(src[i] == 'e' || src[i] == 'E') // exponent
      {
        str[c++] = src[i++]; // write exponent
        resize(str, size, c);

        if(src[i] == '+' || src[i] == '-') // sign
        {
          str[c++] = src[i++]; // write sign
          resize(str, size, c);
        }

        assert(isdigit(src[i])); // digits must follow, regardless of sign
        while(isdigit(src[i])) // write digits
        {
          str[c++] = src[i++];
          resize(str, size, c);
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
        t->floating.cont = num;

        t->floating.isshort = 1;
      }
      else if(suf == 'l') // long double
      {
        sscanf(str, "%f", &num);
        t->floating.cont = num;

        t->floating.islong = 1;
      }
      else // double
      {
        sscanf(str, "%f", &num);
        t->floating.cont = num;
      }

      free(str); // no longer needed
      
      return t;
    }

    throw("this assert should never run");
  }

  else if(src[i] == '"') // string literal
  {
    i++; // skip quotation mark

    // write string into str
    while(quot[i])
    {
      str[c++] = src[i++];
      resize(str, size, c);

      // if(c > size)
      // {
      //   str = realloc(str, size*2);
      //   size *= 2;
      // }
    }
    str[c] = 0;
    i++; // skip closing quotation mark

    unesc(str); // convert escape sequences into the real deal
    
    t->gen.type = STRLIT;
    t->strlit.cont = str;
    t->strlit.len = c+1; // length needs to be remembered, including terminating '\0' (length of the array, not the string)

    return t;
  }

  else if(src[i] == '\'') // character constant
  {
    i++; // skip quote

    // write string into str
    while(quot[i])
    {
      str[c++] = src[i++];
      resize(str, size, c);

      // if(c > size)
      // {
      //   str = realloc(str, size*2);
      //   size *= 2;
      // }
    }
    str[c] = 0;
    i++; // skip closing quotation mark

    unesc(str); // convert escape sequences into the real deal
    assert(strlen(str) == 1); // the final literalized character should be unaccompanied

    unsigned char chr;
    chr = str[0];
    
    t->gen.type = CHAR;
    t->character.cont = chr;

    return t;
  }

  else if(src[i] == ';') // semicolon, easiest case
  {
    t->gen.type = ATOM;
    t->atom.cont = SEMICOLON;
    i++;
    return t;
  }

  // separators
  else if(src[i] == '(') // also sometimes part of operator
  {
    t->gen.type = ATOM;
    t->atom.cont = PARENOP;
    i++;
    return t;
  }
  else if(src[i] == ')') // also sometimes part of operator
  {
    t->gen.type = ATOM;
    t->atom.cont = PARENCL;
    i++;
    return t;
  }
  else if(src[i] == '{')
  {
    t->gen.type = ATOM;
    t->atom.cont = BRACEOP;
    i++;
    return t;
  }
  else if(src[i] == '}')
  {
    t->gen.type = ATOM;
    t->atom.cont = BRACECL;
    i++;
    return t;
  }
  else if(src[i] == '[')
  {
    t->gen.type = ATOM;
    t->atom.cont = BRACKOP;
    i++;
    return t;
  }
  else if(src[i] == ']')
  {
    t->gen.type = ATOM;
    t->atom.cont = BRACKCL;
    i++;
    return t;
  }

  // now for operators
  // sometimes operators do not appear whole
  // e.x. function calls and casts
  // we mark all parens as just parens and leave it to later logic to sort out what's actually going on
  
  t->gen.type = ATOM; // process of elimination

  // note that there are identical unary and binary operators
  // those are determined by later context
  
  if(src[i] == '-') // - -- -> -=
  {
    i++;
    if(src[i] == '>')
    {
      i++;
      t->atom.cont = ARROW;
    }
    else if(src[i] == '-')
    {
      i++;
      t->atom.cont = DEC;
    }
    else if(src[i] == '=')
    {
      i++;
      t->atom.cont = MINEQ;
    }
    else
      t->atom.cont = MIN; // MIN is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '.') // guaranteed now not to be float
  {
    i++;
    t->atom.cont = DOT;
  }

  else if(src[i] == '!') // ! !=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t->atom.cont = NOTEQ;
    }
    else
      t->atom.cont = LOGNOT;
  }

  else if(src[i] == '~')
  {
    i++;
    t->atom.cont = BITNOT;
  }

  else if(src[i] == '+') // + += ++
  {
    i++;
    if(src[i] == '+')
    {
      i++;
      t->atom.cont = INC;
    }
    else if(src[i] == '=')
    {
      i++;
      t->atom.cont = PLUSEQ;
    }
    else
      t->atom.cont = PLUS; // PLUS is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '*') // * *=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t->atom.cont = TIMESEQ;
    }
    else
      t->atom.cont = STAR; // STAR is not a final operator, it's a placeholder before we know if it's unary or binary
  }

  else if(src[i] == '/') // / /*
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t->atom.cont = DIVEQ;
    }
    else
      t->atom.cont = DIV;
  }

  else if(src[i] == '%') // % %=
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t->atom.cont = MODEQ;
    }
    else
      t->atom.cont = MOD;
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
        t->atom.cont = SHLEQ;
      }
      else
        t->atom.cont = SHL;
    }
    else if(src[i] == '=')
    {
      i++;
      t->atom.cont = LEQ;
    }
    else
      t->atom.cont = LESS;
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
        t->atom.cont = SHREQ;
      }
      else
        t->atom.cont = SHR;
    }
    else if(src[i] == '=')
    {
      i++;
      t->atom.cont = GEQ;
    }
    else
      t->atom.cont = GREAT;
  }

  else if(src[i] == '=') // = ==
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t->atom.cont = EQEQ;
    }
    else
      t->atom.cont = EQ;
  }

  else if(src[i] == '&') // & && &=
  {
    i++;
    if(src[i] == '&')
    {
      i++;
      t->atom.cont = LOGAND;
    }
    else if(src[i] == '=')
    {
      i++;
      t->atom.cont = ANDEQ;
    }
    else
      t->atom.cont = BITAND;
  }

  else if(src[i] == '|') // | || |=
  {
    i++;
    if(src[i] == '|')
    {
      i++;
      t->atom.cont = LOGOR;
    }
    else if(src[i] == '=')
    {
      i++;
      t->atom.cont = OREQ;
    }
    else
      t->atom.cont = BITOR;
  }

  else if(src[i] == '^')
  {
    i++;
    if(src[i] == '=')
    {
      i++;
      t->atom.cont = XOREQ;
    }
    else
      t->atom.cont = BITXOR;
  }

  else if(src[i] == ',') // can be separator or operator. assume separator for now
  {
    i++;
    t->atom.cont = COMMA;
  }

  else if(src[i] == ':') // can be part of label or part of ternary
  {
    i++;
    t->atom.cont = COLON;
  }

  else if(src[i] == '?') // part of ternary (incomplete, must complete later)
  {
    i++;
    t->atom.cont = QUESTION;
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
    // if(quot[i])
    //   putchar(toupper(src[i]));
    // else
    //   putchar(src[i]);
    assert(strchr(banned, src[i]) == NULL || quot[i]);
  }
}

list *proctokens(char *src, char *esc, char *quot)
{
  list *trans_unit = makelist(sizeof(token));

  // turn text into tokens
  do
  {
    // resize(trans_unit, tsize, tcount);
    token *t = nexttok(src, esc, quot);
    append(trans_unit, t);
    free(t);
    // nexttok(src, esc, quot, trans_unit+tcount);
    // puttok(trans_unit[tcount]);
    // puttok(*(token *)last(trans_unit));

  }
  while(((token *)last(trans_unit))->gen.type != NOTOK);
  
  return trans_unit;
}


//{{{1 tools
// all chars and ints are integral types
int isintegral(ctype ct)
{
  if(ct->gen.type != TM_DAT)
    return 0;
  int dt = ct->dat.dt;
  return dt == CHAR_T || dt == UCHAR_T || dt == INT_T || dt == UINT_T || dt == SINT_T || dt == LINT_T || dt == USINT_T || dt == ULINT_T;
}

// check if integral type is of the signed variety
int issigned(ctype ct)
{
  assert(isintegral(ct));
  int dt = ct->dat.dt;
  return dt == CHAR_T || dt == INT_T || dt == SINT_T || dt == LINT_T;
}

// integral or floating
int isarith(ctype ct)
{
  if(ct->gen.type != TM_DAT)
    return 0;
  int dt = ct->dat.dt;
  return isintegral(ct) || dt == FLOAT_T || dt == DUB_T || dt == LDUB_T;
}

int isscalar(ctype ct)
{
  if(isarith(ct))
    return 1;

  return ct->gen.type == TM_PTR;
}

int eistype(expr *e, int type);
int atomisasgnop(int);

void puttypemod(typemod ts);

void putctype(ctype ct)
{
  // typemods
  int i = 0;
  for(; ct[i].gen.type != TM_DAT; i++)
  {
    puttypemod(ct[i]);
  }

  // dattype
  if(ct[i].dat.isconst)
    printf("const ");
  if(ct[i].dat.isvolat)
    printf("volatile ");
  
  printf("%s ", hrdt[ct[i].dat.dt]);

  // store spec
  // if(ct->storespec == EXTERN_S)
  //   printf("extern ");
  // else if(ct->storespec == STATIC_S)
  //   printf("static ");

  // if(ct->isconst)
  //   printf("const ");
  // if(ct->isvolat)
  //   printf("volat ");

  // printf("%s ", hrdt[ct->dattype]);
  fflush(stdout);
}

void putdecl(decl *dcl)
{
  void putinit(struct init *e, int space);

  if(dcl->ident)
  {
    printf("%s: ", dcl->ident);
  }

  // if(dcl->typemods)
  // {
  //   typemod *tms = (typemod *) dcl->typemods->cont;
  //   int tmlen = dcl->typemods->n;
  //   for(int j = 0; j < tmlen; j++)
  //   {
  //     puttypemod(tms[j]);
  //   }
  // }

  if(dcl->storespec != NOSPEC)
  {
    printf("%s ", keywords[dcl->storespec]);
  }

  // int *tqs = (int *) dcl->typequals->cont;
  // int len = dcl->typequals->n;
  
  // for(int i = 0; i < len; i++)
  //   printf("%s ", keywords[tqs[i]]);

  // printf("%s ", hrdt[dcl->dattype]);

  if(dcl->ct)
  {
    putctype(dcl->ct);
    puts("");
  }

  if(dcl->init)
  {
    printf(":= \n");
    putinit(dcl->init, 2);
  }
  else
    putchar('\n');

  if(dcl->fundef)
  {
    printf("with definition ___\n");
  }
  else
    putchar('\n');


  // int *tss = (int *) dcl->typespecs->cont;
  // len = dcl->typespecs->n;
  // for(int i = 0; i < len; i++)
  // {
  //   printf("%s ", keywords[tss[i]]);
  // }
}

int putparams(ctype ct)
{
  assert(ct->gen.type == TM_FUNC);
  putd(ct->func.np);
  for(int i = 0; i < ct->func.np; i++)
  {
    putctype(ct->func.params[i].ct);
    printf(", ");
  }
  nline();
  nline();
}

void putexpr(expr *e)
{
  helpputexpr(e, 0);
}

void helpputexpr(expr *e, int space)
{
  // int *types = (int *) e->type->cont;
  // int nt = e->type->n;

  for(int i = 0; i < space; i++)
  {
    putchar(' ');
  }

  printf("%s ", hr_expr[e->type]);
  // for(int i = 0; i < nt; i++)
  // {
  //   printf("%s ", hr_expr[types[i]]);
  // }

  // putd(e->optype);
  // putd(SUB_O);
  // if(isasgnop(e->optype))
  // {
  //   printf(": %s", hrat[e->optype]);
  // }
  // else
  // {
  if(!eistype(e, TYPENAME) && e->optype != -1)
    printf(": %s", hropt[e->optype]);
  // }

  if(eistype(e, PRIM_E))
  {
    printf(" : ");
    // puttok(*e->tok);
    if(e->optype == STRING_O)
    {
      // for(int i = 0; i < e->ct->arr.len; i++)
      // {
      //   printf("%02x ", e->strlit[i]);
      // }
    }
    else
      printf("0x%08x", e->dat);
  }
  // if(eistype(e, TYPENAME))
  if(!eistype(e, ARGLIST)) // arglists do not have a ctype
  {
    printf(" : ");
    putctype(e->ct);
  }
  printf(" : %d ", e->numargs);
  putchar('\n');


  for(int i = 0; i < e->numargs; i++)
  {
    helpputexpr(e->args[i], space+2);
  }
}

void putinit(struct init *init, int space)
{
  for(int i = 0; i < space; i++)
    putchar(' ');

  if(!init->islist)
  {
    helpputexpr(init->e, space-2);
  }
  else
  {
    printf("list of length %d\n", init->len);
    for(int i = 0; i < init->len; i++)
    {
      putinit(init->lst[i], space+2);
    }
  }
}

// not as severe as sever; does not give error if it encounters NULL
void trysever(link *l)
{
  if(!l) return;
  if(l->left) l->left->right = NULL;
  if(l->right) l->right->left = NULL;
}

#define sever(l) {  testerr(l->left, "couldn't sever left");  l->left->right = NULL;  testerr(l->right, "couldn't sever right");  l->right->left = NULL;}
// void sever(link *l)
// {
//   // if(!l) return;
//   assert(l);
//   assert(l->left);
//   l->left->right = NULL;
//   assert(l->right);
//   l->right->left = NULL;
// }

int corresp(int num, int *a, int *b, int x)
{
  for(int i = 0; i < num; i++)
  {
    if(a[i] == x)
      return b[i];
  }
  // testerr(0, "corresp: x not in a");
  throw("corresp: x not in a");
}

void etypeadd(expr *e, int type)
{
  // if(e->type == NULL)
  //   e->type = makeset(sizeof(int));
  // setins(e->type, &type);
  e->type = type;
}

// DONE eistype is sometimes broken, sometimes not
int eistype(expr *e, int type)
{
  // if(e->type == NULL)
  //   return 0;

  // else
  // {
  // // putd(type);
  //   return inset(e->type, &type);
  // }
  return e->type == type;
}

// int leistype(link *l, int type)
// {
//   return l != NULL && l->type == EXPR_L && eistype(l->cont.exp, type);
// }


token *ll2tokl(link *ll) // linked list to NOTOK-terminated token list
{
  link *start = ll; // save
  int len = 0; // find length
  while(ll)
  {
    len++;
    ll = ll->right;
  }

  ll = start; // reset
  
  // allocate enough for tokens + NOTOK
  token *tokl = malloc(sizeof(token) * (len+1));
  int i = 0;
  
  while(ll) // go through and convert to tokens
  {
    if(ll->type == TOK_L) // already token, transfer
      tokl[i++] = *ll->cont.tok;
    else // expression
    {
      // expression parsing has changed, this should never happen
      throw("links should not contain expression");
      // tokl[i++] = *ll->cont.exp->tok;
    }
    ll = ll->right;
  }

  // NOTOK
  token nt;
  nt.gen.type = NOTOK;
  tokl[i] = nt;

  return tokl;
}

// token subarray -> linked list -> expr
expr *tokl2expr(token *toks, int lo, int hi)
{
  // inclusive
  assert(lo <= hi); // make sure nonempty. empty cases should be handled by the caller (e.x. in a for loop, an empty condition evaluates to true

  link *ll = tokl2ll(toks + lo, hi - lo + 1);
  expr *e = parseexpr(ll);
  
  return e;
}

link *tokl2ll(token *tokl, int len) // NOTOK-terminated token list to linked list
{
  // int len = 0;
  if(len == -1) // determine len via NOTOK
  {
    len = 0;
    while(!(tokl[len].gen.type == NOTOK)) len++;
  }
  assert(len != 0);
  
  link *ll = malloc(sizeof(link) * len);

  for(int i = 0; i < len; i++)
  {
    ll[i].type = TOK_L;
    ll[i].cont.tok = tokl+i;
  }
  
  ll[0].left = NULL;
  ll[len-1].right = NULL;
  for(int i = 0; i < len-1; i++)
  {
    ll[i].right = ll+i+1;
    ll[i+1].left = ll+i;
  }

  return ll;
}

int iskeyword(token *t, enum keyword k)
{
  return t != NULL && t->gen.type == KEYWORD && t->keyword.cont == k;
}
int tiskeyword(token t, enum keyword k)
{
  return t.gen.type == KEYWORD && t.keyword.cont == k;
}
int isatom(token *t, enum atom_type a)
{
  return t != NULL && t->gen.type == ATOM && t->atom.cont == a;
}
int tisatom(token t, enum atom_type a)
{
  return t.gen.type == ATOM && t.atom.cont == a;
}

// int lisop(link *l, int o)
// {
//   return l != NULL && l->type == EXPR_L && l->cont.exp->optype == o;
// }
// int lisexpr(link *l, expr_type e)
// {
//   return l != NULL && l->type == EXPR_L && l->cont.exp->type == e;
// }
int lisatom(link *l, enum atom_type a)
{
  return l != NULL && l->type == TOK_L && isatom(l->cont.tok, a);
}

int listok(link *l, enum tok_type t)
{
  return l != NULL && l->type == TOK_L && l->cont.tok->gen.type == t;
}

// find matching paren, brace, matching ?:, etc.
link *findmatch(link *start, int dir, int inc, int dec)
{
  int dep = 1;
  link *curl = start;
  do
  {
    curl = (dir == RIGHT) ? curl->right : curl->left;

    if(lisatom(curl, inc)) dep++;
    if(lisatom(curl, dec)) dep--;
    // putd(dep);
    // // puttok(*curl->cont.tok);
    // nline();
    assert(dep >= 0);
    assert(curl != NULL);

  } while(dep > 0);
  
  return curl;
}


int lisunaryop(link *l) // & * + - ~ !
{
  return lisatom(l, BITAND) ||
    lisatom(l, STAR) ||
    lisatom(l, PLUS) ||
    lisatom(l, MIN) ||
    lisatom(l, LOGNOT) ||
    lisatom(l, BITNOT);
}


// int gettypespec(token t) // get type specifier
// {
//   if(t.gen.type == KEYWORD)
//   {
//     enum keyword k = t.keyword.cont;
//     if(k == K_VOID || k == K_CHAR || k == K_SHORT || k == K_INT || k == K_LONG || k == K_FLOAT || k == K_DOUBLE || k == K_SIGNED || k == K_UNSIGNED)
//     {
//       return k;
//     }
//     else return -1;
//   }
//   // TODO user-defined specifiers, can be identifiers
//   else return -1;

// }

// int gettypequal(token t) // get type qualifier
// {
//   if(t.gen.type == KEYWORD)
//   {
//     enum keyword k = t.keyword.cont;
//     if(k == K_VOLATILE || k == K_CONST)
//     {
//       return k;
//     }
//     else return -1;
//   }
//   return -1;
// }

// int getstorespec(token t) // get storage class specifier
// {
//   if(t.gen.type == KEYWORD)
//   {
//     enum keyword k = t.keyword.cont;
//     if(k == K_AUTO || k == K_REGISTER || k == K_STATIC || k == K_EXTERN || k == K_TYPEDEF)
//     {
//       return k;
//     }
//     return -1;
//   }
//   return -1;
// }

// int isdeclspec(token t) // get declaration specifier, -1 if it's not that
// {
//   int x;
//   if((x = gettypespec(t)) != -1)
//   {
//     return 1;
//   }
//   if((x = getstorespec(t)) != -1)
//   {
//     return 1;
//   }
//   if((x = gettypequal(t)) != -1)
//   {
//     return 1;
//   }
//   return 0;
// }

// types, expressions

// parse the list of parameters in a function type
// we don't support ... parameters
list *parseparamlist(link *start)
{
  link *nexttoplevel(link *start, int dir, int num, int *atoms);
  int gettypemods(token *toks, int lo, int hi, int abs, char **s, typemod **dat);

  assert(start);
  leftend(start);

  static int cl[] = {COMMA};
  link *comma;
  list *paramlist = makelist(sizeof(decl));

  do
  {
    comma = nexttoplevel(start, RIGHT, 1, cl);
    assert(comma != start); // no empty
    if(comma) sever(comma);

    // parse declspecs and typemods
    token *toks = ll2tokl(start);
    int i = 0;
    decl *dcl = getdeclspecs(toks, &i);
    ctype ct = dcl->ct;


    // param->typemods = makelist(sizeof(typemod));
    // list *l = makelist(sizeof(typemod));
    gettypemods(toks, i, -1, -1, &dcl->ident, &ct);
    // ct->tms = (typemod *) l->cont;

    // puts("huei");
    // putctype(ct);nline();

    dcl->ct = ct;
    // the only reason we use decls is to store the possible ident

    // // if not abstract, remove the identifier
    // typemod first = * (typemod *) param->typemods->cont;
    // if(first.gen.type == IDENT)
    //   rem_front(param->typemods);

    free(toks);

    append(paramlist, dcl);
    
    if(comma) start = comma->right;
  } while(comma);

  return paramlist;
}


// recursive function that interprets a declarator by getting the type modifiers in order
// returns hi + 1 (where to pick up next)
int gettypemods(token *toks, int lo, int hi, int abs, char **s, ctype *dat)
{
  int helpgettypemods(token *toks, int lo, int hi, list *l, int abs);

  // parse typemods, write into l
  list *l = makelist(sizeof(typemod));
  int i = helpgettypemods(toks, lo, hi, l, abs);
  
  // typemod tm;
  // tm.gen.type = TM_NONE;

  

  // typemods are parsed from the outside in, so now we flip that
  reverse(l);
  // append data type onto end (e.g. const int)
  append(l, *dat);

  if(((typemod*)l->cont)[0].gen.type == TM_IDENT) // if not abstract
  {
    *s = ((typemod *)l->cont)[0].ident.name; // write pointer to ident name into s
    rem_front(l); // then remove the identifier
  }
  else // otherwise indicate abstract
  {
    if(s) // if the caller knows it's abstract already, it will pass NULL
      *s = NULL;
  }


  // write back into dat
  *dat = (typemod *) l->cont;

  // check ctype for validity
  // we cannot check for incomplete type, because ex. function prototypes allow that. however we can always check for validity, and every type that appears in source code must pass through this function
  assert(validct(*dat));
  
  return i;
}

int helpgettypemods(token *toks, int lo, int hi, list *l, int abs)
{
  // here();
  // int i = lo;
  // while(toks[i].gen.type != NOTOK)
  // {
  //   puttok(toks[i]);
  //   i++;
  // }
  // nline();

  // initial recursive call, must find right side of declarator
  // we also verify that parentheses are balanced here, just because we can
  if(hi == -1)
  {
    if(toks[lo].gen.type == NOTOK) // no modifiers, abstract declarator
    {
      return lo;
    }

    hi = lo; // start from here
    int parendep = 0;
    
    for(;; hi++) // possible tokens: ( * const volatile
    {
      if(isatom(toks+hi, STAR))
      {
        continue;
      }
      if(isatom(toks+hi, PARENOP)) // if (, remember it
      {
        if(isdeclspec(toks[hi+1]) || isatom(toks+hi, PARENCL))
            break;
        parendep++;
        continue;
      }
      if(iskeyword(toks+hi, K_VOLATILE) || iskeyword(toks+hi, K_CONST))
      {
        continue;
      }

      // reached identifier
      break;
    }
    // assert(toks[hi].gen.type == IDENT); // assert identifier
    // hi++; // pass identifier
    // ^^ abstract declarators have no identifier
    // putd(1);


    // check for identifier according to abs
    if(abs == 0) // must not be abstract
    {
      assert(toks[hi].gen.type == IDENT);
      hi++;
    }
    else if(abs == 1) // must be abstract
    {
      assert(toks[hi].gen.type != IDENT);
    }
    else if(abs == -1) // means either is allowed, determine which it is now
    {
      if(toks[hi].gen.type == IDENT)
      {
        abs = 0;
        hi++;
      }
      else abs = 1;
    }
    // if(toks[hi].gen.type == IDENT) // not abstract, has identifier
    // {
    //   abs = 0;
    //   hi++; // pass over
    // }

    // possible things: [constant-expression_opt] (parameter-type-list) )
    for(;;) // all incrementing is now done manually, because it's more complex
    {
      if(isatom(toks+hi, PARENCL)) // )
      {
        parendep--;
        assert(parendep >= 0); // make sure we're not going under
        hi++;
        continue;
      }

      if(isatom(toks+hi, BRACKOP)) // [
      {
        // find matching bracket
        // i don't think brackets can appear in constant expressions, but we still have to accommodate every expression before checking later if it's constant
        int brackdep = 0;
        do
        {
          if(isatom(toks+hi, BRACKOP)) brackdep++;
          if(isatom(toks+hi, BRACKCL)) brackdep--;
          assert(parendep >= 0 && toks[hi].gen.type != NOTOK);
          hi++;
        } while(brackdep);

        continue;
      }
      // puttok(toks[hi]);nline();

      if(isatom(toks+hi, PARENOP)) // (
      {
        // find matching paren
        int curdep = parendep;
        do
        {
          if(isatom(toks+hi, PARENOP)) parendep++;
          if(isatom(toks+hi, PARENCL)) parendep--;
          assert(parendep >= 0 && toks[hi].gen.type != NOTOK);
          hi++;
        } while(curdep != parendep);

        continue;
      }

      assert(parendep == 0); // must be depth 0, then the declarator is done
      hi--; // step back onto end of declarator
      break;
    }
  }

  // aside from hi==-1, hi < lo indicates we are done
  if(hi < lo)
    return hi+1;

  // get one typemod then recurse
  typemod *tmod = calloc(1,sizeof(typemod));

  if(isatom(toks+lo, STAR)) // pointer time
  {
    tmod->gen.type = TM_PTR;

    // look for qualifiers
    lo++;
    for(;; lo++)
    {
      if(iskeyword(toks+lo, K_VOLATILE))
      {
        tmod->ptr.isvolat = 1;
      }
      else if(iskeyword(toks+lo, K_CONST))
      {
        tmod->ptr.isconst = 1;
      }
      else // end of qualifiers
        break;
    }

    append(l, tmod); // add to list of typemods
    free(tmod); // append uses memcpy so this is no longer needed
    
    if(abs && toks[lo].gen.type == NOTOK) // if abstract, then there might be nothing after this; don't recurse in that case
    {
      return hi+1;
    }

    helpgettypemods(toks, lo, hi, l, abs);

    return hi+1;
  }
  
  if(isatom(toks+lo, PARENOP) || toks[lo].gen.type == IDENT) // direct declarator time, must look from other side
  {
    if(isatom(toks+hi, PARENCL)) // either function or just parenthesis
    {
      // find matching paren to figure out
      int parendep = 0;
      int i = hi; // hello
      do
      {
        if(isatom(toks+i, PARENCL)) parendep++;
        if(isatom(toks+i, PARENOP)) parendep--;
        assert(parendep >= 0 && i >= 0);
        i--;
      } while(parendep != 0);
      i++; // back onto last (

      // must determine whether enclosing parentheses or function parentheses
      int fun = 1;

      if(i == lo) // back where we started
      {
        if(abs) // either enclosing parentheses or function parenthesis; must inspect contents to determine which
        {
          if(lo + 1 == hi) // empty parentheses, must be function
          {
            fun = 1;
          }

          else if(isdeclspec(toks[lo+1])) // only possible start of parameter declaration
          {
            fun = 1;
          }

          else fun = 0; // otherwise must be enclosing
        }
        else fun = 0; // not abs, back where we started -> enclosing

        if(!fun) // enclosing
        {
          free(tmod); // wasn't needed
          // recurse without the paren
          helpgettypemods(toks, lo+1, hi-1, l, abs);

          return hi+1;
        }
      }

      if(fun) // it's a function
      {
        tmod->gen.type = TM_FUNC;

        // store parameter list
        if(i+1 == hi) // empty list, means params unspecified
        {
          tmod->func.params = NULL; // indicate this with NULL
          tmod->func.np = -1;
        }
        else
        {
          link *ll = tokl2ll(toks+i+1, hi-i-1);
    // puts("hi");
    // putll(ll);
    // nline();
    // nline();
          list *plist = parseparamlist(ll);

          decl *params = (decl *) plist->cont;
          tmod->func.params = params;
          int np = plist->n;
          tmod->func.np = np;

    // putparams(tmod);

    // for(int i = 0 ; i <np; i++)
    // {
    //   putctype(params[i].ct);
    //   nline();
    // }
    // nline();
    // nline();
    // nline();
    // nline();

          // check for void
          for(int j = 0; j < np; j++)
          {
            if(ctisdt(params[j].ct, VOID_T) && params[j].ident == NULL) // abstract type void -> no params
            {
              assert(np == 1); // void must be only param

              // modify to have 0 params
              tmod->func.params = NULL;
              tmod->func.np = 0;
            }
          }

          // TODO check all given types for completeness
        }

        append(l, tmod);
        free(tmod);
        if(!abs || i != lo) helpgettypemods(toks, lo, i-1, l, abs); // must recurse further
        
        return hi+1;
      }
    }

    if(isatom(toks+hi, BRACKCL)) // array
    {
      // find matching bracket
      int brackdep = 0;
      int i = hi;
      do
      {
        if(isatom(toks+i, BRACKCL)) brackdep++;
        if(isatom(toks+i, BRACKOP)) brackdep--;
        assert(brackdep >= 0 && i >= 0);
        i--;
      } while(brackdep != 0);
      i++; // back onto last [

      tmod->gen.type = TM_ARR;

      if(hi-i == 1) // empty length, incomplete type
        tmod->arr.len = -1;
      else // otherwise calculate type now
      {
        expr *e = tokl2expr(toks, i+1, hi-1);
        assert(isintegral(e->ct));
        sdword len = evalconstintexpr(e);
        assert(len >= 1);
        // putd(len);
        tmod->arr.len = len;
      }
      
      // tmod->arr.len = -1; // signify incomplete type for now
      // TODO evaluate and store length (should be constant expression)
      // also make sure len > 0

      append(l, tmod);
      free(tmod);
      if(!abs || i != lo) helpgettypemods(toks, lo, i-1, l, abs); // must recurse further

      // gettypemods(toks, lo, i-1, l, abs);
      // no need to recurse; we're done because it's abstract and the [] was at the start
      // ^^^ turns out this was incorrect
      // spaghetti code

      return hi+1;
    }

    // else, we should have reached the identifier (should not be possible for abstract declarator to reach this part of the code)
    // putd(lo);
    // putd(hi);
    assert(lo == hi);
    assert(toks[lo].gen.type == IDENT);

    tmod->gen.type = TM_IDENT;
    tmod->ident.name = toks[lo].ident.cont;

    append(l, tmod);
    free(tmod);

    return hi+1;
  }

  else // then must be abstract declarator
  {
    // moreover, functions are dealt with above, so it must be an array
    assert(abs);
    putd(lo);
    putd(hi);
    assert(isatom(toks+lo, BRACKOP));

    // find matching bracket
    int brackdep = 0;
    int i = lo;
    do
    {
      if(isatom(toks+i, BRACKOP)) brackdep++;
      if(isatom(toks+i, BRACKCL)) brackdep--;
      assert(brackdep >= 0 && i >= 0);
      i++;
    } while(brackdep != 0);
    i--; // back onto last ]

    // TODO evaluate and store length (should be constant expression)
    tmod->gen.type = TM_ARR;
    append(l, tmod);
    free(tmod);
    // gettypemods(toks, lo, i-1, l, abs);

    return hi+1;
  }
}

void puttypemod(typemod ts)
{
  enum tmt type = ts.gen.type;
  if(type == TM_IDENT)
  {
    printf("%s is a ", ts.ident.name);
  }
  if(type == TM_PTR)
  {
    if(ts.ptr.isconst)
      printf("constant ");
    if(ts.ptr.isvolat)
      printf("volatile ");
    printf("pointer to ");
  }
  if(type == TM_ARR)
  {
    printf("array of ");
  }
  if(type == TM_FUNC)
  {
    printf("function returning ");
  }
}


// return 0 if toplevel VOID_T (still gives error for void arr[3], for example)
int safesizeoftype(ctype ct)
{
  if(ctisdt(ct, VOID_T))
    return 0;

  return sizeoftype(ct);
}

// int helpsizeoftype(int dt, typemod *tms)
int sizeoftype(ctype ct)
{
  int dtsize(int dt);

  int tmtype = ct->gen.type; // type of first typemod
  
  assert(tmtype != TM_FUNC); // functions have no size
  switch(tmtype)
  {
    // should not occur under our system
    case TM_IDENT:
      throw("TM_IDENT not allowed outside of initial typemod parsing");
      // return helpsizeoftype(dt, tms+1); // not abstract declarator, just skip identifier

    case TM_DAT:
      return dtsize(ct->dat.dt); // no more typemods, just size of dt

    case TM_PTR:
      return PTR_SIZE; // all pointers have same size regardless of what they point to

    case TM_ARR:
      assert(ct->arr.len != -1); // incomplete type
      return ct->arr.len * sizeoftype(ct+1);
  }

  throw("should not reach here");
}

// get size of dattype
int dtsize(int dt)
{
// TODO struct, enum, union, etc.
  assert(dt != VOID_T);

  switch(dt)
  {
    case CHAR_T: return CHAR_SIZE;
    case UCHAR_T: return CHAR_SIZE;

    case INT_T: return INT_SIZE;
    case UINT_T: return INT_SIZE;
    case SINT_T: return INT_SIZE;
    case LINT_T: return INT_SIZE;
    case USINT_T: return INT_SIZE;
    case ULINT_T: return INT_SIZE;

    case FLOAT_T: return FLOAT_SIZE;
    case DUB_T: return FLOAT_SIZE;
    case LDUB_T: return FLOAT_SIZE;
    default: throw("invalid dattype"); return -1;
  }
}

// type specifiers work weirdly, so we perform some checks to make sure it's sane, then turn the multiple specs into a single enum
// TODO replace this with just an int
int proctypespecs(set *ts)
{
  int islong = 0;
  int isshort = 0;
  int issigned = 1; // under this implementation all integral types are signed unless specified unsigned
  
  // set *ts = ct->typespecs;

  // TODO incorporate typedefs, structs/unions/enums
  static int basetypes[] = {K_VOID, K_CHAR, K_INT, K_FLOAT, K_DOUBLE, };
  int btlen = sizeof(basetypes)/sizeof(basetypes[0]);

  int type = K_INT; // default
  // find the basetype and make sure it's unique
  for(int i = 0; i < btlen; i++)
  {
    if(intinset(ts, basetypes[i]))
      type = basetypes[i];
  }
  // make sure only one basetype
  for(int i = 0; i < btlen; i++)
  {
    if(basetypes[i] != type)
    {
      assert(!intinset(ts, basetypes[i]));
    }
  }

  int isinteg = (type == K_CHAR || type == K_INT);

  // only one of short and long allowed
  if(intinset(ts, K_LONG))
  {
    assert(type == K_INT || type == K_DOUBLE);
    assert(!intinset(ts, K_SHORT));
    islong = 1;
  }
  if(intinset(ts, K_SHORT))
  {
    assert(type == K_INT);
    assert(!intinset(ts, K_LONG));
    isshort = 1;
  }

  // only one of signed and unsigned allowed
  if(intinset(ts, K_SIGNED))
  {
    assert(isinteg);
    assert(!intinset(ts, K_UNSIGNED));
    issigned = 1;
  }
  if(intinset(ts, K_UNSIGNED))
  {
    assert(isinteg);
    assert(!intinset(ts, K_SIGNED));
    issigned = 0;
  }

  if(type == K_VOID) return VOID_T;
  else if(type == K_CHAR)
  {
    if(issigned) return CHAR_T;
    else return UCHAR_T;
  }
  else if(type == K_INT)
  {
    if(issigned)
    {
      if(isshort) return SINT_T;
      else if(islong) return LINT_T;
      else return INT_T;
    }
    else
    {
      if(isshort) return USINT_T;
      else if(islong) return ULINT_T;
      else return UINT_T;
    }
  }
  else if(type == K_FLOAT)
  {
    puts("Error: floating point not supported yet");
    exit(1);
    return FLOAT_T;
  }
  else if(type == K_DOUBLE)
  {
    puts("Error: floating point not supported yet");
    exit(1);
    if(islong) return LDUB_T;
    else return DUB_T;
  }

  throw("should not reach here");
}

//{{{1 decls
// gets a single decl spec in form of integer
int getspec(token t)
{
  // TODO custom types
  
  if(t.gen.type == KEYWORD) // all builtin decl specs are keywords
  {
    return t.keyword.cont;
  }
  else
    return K_NONE;
}

int istypespec(int k)
{
  return k == K_VOID || k == K_CHAR || k == K_SHORT || k == K_INT || k == K_LONG || k == K_FLOAT || k == K_DOUBLE || k == K_SIGNED || k == K_UNSIGNED;
}

int istypequal(int k)
{
  return k == K_VOLATILE || k == K_CONST;
}

int isstorespec(int k)
{
  return k == K_AUTO || k == K_REGISTER || k == K_STATIC || k == K_EXTERN || k == K_TYPEDEF;
}

int isdeclspec(token t)
{
  if(t.gen.type != KEYWORD)
    return 0;

  int k = t.keyword.cont;
  return isstorespec(k) || istypespec(k) || istypequal(k);
}

// get storespecs, typequals, and typespecs from the front of a declaration
// storespecs go into the dcl
// typequals, typespecs get put into a typemod list
decl *getdeclspecs(token *toks, int *i)
{
  decl *dcl = calloc(1, sizeof(decl));
  // ctype ct;

  // ct is an array of typemods
  typemod *ct = calloc(1, sizeof(typemod));
  // first typemod is dat
  ct->gen.type = TM_DAT;
  dcl->ct = ct;
  
  dcl->storespec = NOSPEC; // 0
  set *typespecs = makeset(sizeof(int)); // temporary storage for typespecs, to be converted  into dattype

  // list *typemods = makelist(sizeof(typemod));
  // ct->tms = NULL;

  int n = *i; // save starting point for future reference (e.x. checking typedef)
  token t;
  int k;

  // read declaration specifiers
  for(;; (*i)++)
  {
    t = toks[*i];
    k = getspec(t);
    
    if(istypespec(k))
    {
      assert(!setins(typespecs, &k)); // no duplicate type specifiers allowed
    }

    else if(k == K_CONST) // duplicate quals allowed
    {
      ct->dat.isconst = 1;
    }
    else if(k == K_VOLATILE)
    {
      ct->dat.isvolat = 1;
    }

    else if(isstorespec(k))
    {
      assert(dcl->storespec == NOSPEC); // only 1 storespec allowed
      dcl->storespec = specmap[k];
    }
    else break; // end of declaration specifiers
  }

  // condense typespecs into one integer
  ct->dat.dt = proctypespecs(typespecs);

  // TODO perform checks on the specifiers to make sure they're allowed
  /*
    - [ ] auto and register only in functions
    - [x] conflicting type specifiers. short, long, unsigned, signed, etc.
    - [x] only one storage class allowed
    - [ ] (warning) typedef must be at beginning of declaration
    - [ ] functions inside a function are extern, functions declared outside are static with external linkage
    - [ ] etc.
    - [ ] static objects/arrays must be initialized with constant expressions
    - [ ] technically, list members must be constant expressions even if auto or register

    - [ ] empty type specs -> int
     */

  // int temp = K_TYPEDEF; // because of how sets are implemented we need an address
  if(dcl->storespec == K_TYPEDEF) // special case
  {
    // TODO
  }

  // in the case of enum, struct, or union, the type is not done:

  // else if(struct or union specifier) TODO
  

  // else if(enum specifier) TODO

  return dcl;
}


struct init *parseinit(link *start)
{
  assert(start);
  leftend(start);

  struct init *init = malloc(sizeof(struct init));
  if(lisatom(start, BRACEOP)) // {initializer-list}
  {
    // initialize init accordingly
    init->islist = 1;
    init->e = NULL;
    // we will initialize init->lst and init->len once we know the length
    // here();

    link *end = start;
    rightend(end);
    assert(lisatom(end, BRACECL)); // opening brace needs closing brace
    assert(start->right != end); // no empty initializer braces under ISO C
    // here();
    
    // remove surrounding braces
    start = start->right;
    start->left = NULL;
    end = end->left;
    end->right = NULL;

    // here();
    if(lisatom(end, COMMA)) // optional ending comma: remove
    {
      assert(start != end); // comma must not be alone; something must precede it
      end = end->left;
      end->right = NULL;
    }

    // the following is very similar to parsearglist(), except that this is recursive
    
    // count commas
    int len = 1;
    link *temp = start;
    static int cl[] = {COMMA};
    while((temp = nexttoplevel(temp, RIGHT, 1, cl)) != NULL)
    {
      len++;
      temp = temp->right;
    }

    // here();
    // complete initialization
    init->len = len;
    init->lst = malloc(sizeof(struct init *) * len);
    
    // parse subinits
    link *comma;
    for(int i = 0; i < len; i++)
    {
      // find next comma
      comma = nexttoplevel(start, RIGHT, 1, cl);
      assert(comma != start);

      // separate
      if(comma) // if at the end no severing needed
        sever(comma);
      start->left = NULL;
      
      struct init *subinit = parseinit(start);
      assert(subinit);
      if(comma)
        start = comma->right;
      
      // add to list
      init->lst[i] = subinit;
    }
  }

  else // assignment expression
  {
    init->islist = 0;
    init->lst = NULL;
    init->len = 0;

    expr *e = parseasgnexpr(start);
    assert(e);
    init->e = e;
  }

  return init;
}



// read first declaration from array of tokens, and do things about it
// returns NULL if runs into NOTOK
decl * parsedecl(token *toks, int *i, int *sc)
{
  // declaration *decl = malloc(sizeof(decl));
  
  // static int i = 0;
  static decl *specs = NULL; // contains decl specs, not typemods
  static int first = 1; // indicates if this can be a function definition (must be first and only declarator)
  // when we encounter a semicolon we reset it to NULL, indicating that the decl-spec decl-init grouping has ended (e.x. int a=5, *b; is one grouping)

  assert(toks);
  // if(!toks) // reset if passed NULL
  // {
  //   i = 0;
  //   specs = NULL;
  //   return NULL; // return value doesn't matter
  // }

  if(toks[*i].gen.type == NOTOK) // end of token stream
  {
    return NULL;
  }
  assert(!isatom(toks+*i, SEMICOLON)); // not allowed in C90
  
  if(!specs) // read new decl specs
  {
    first = 1; // start anew
    specs = getdeclspecs(toks, i); // parse declaration specifiers and move i forward past them all
  }
  
  // now deal with a single declarator + optional initializer, or function definition


  // get the declarator, put everything into a new decl including a copy of the specs
  // list *l = makelist(sizeof(typemod));
  char *ident;
  decl *dcl = malloc(sizeof(decl));
  memcpy(dcl, specs, sizeof(decl));
  *i = gettypemods(toks, *i, -1, 0, &ident, &dcl->ct); // parse one declarator and move i forward accordingly, write type into dcl->ct
  assert(ident); // no abstract declarators
  dcl->ident = ident;

  int tmlen = getctlen(dcl->ct);

// if(dcl->ct->gen.type == TM_FUNC)
// {
//   putparams(dcl->ct);
// }

  if(isatom(toks+ *i, BRACEOP)) // function definition
  {
    // make sure first place
    assert(first);
    first = 0; // claim first place

    // check if valid function declarator: TM_FUNC, return type
    assert(tmlen >= 2);
    assert(dcl->ct->gen.type == TM_FUNC);

    // put the range of the whole function definition into fd
    struct stat *fd = malloc(sizeof(struct stat));
    fd->toks = toks;
    fd->lo = *i;

    // find matching brace
    int bracedep = 0;
    do
    {
      if(isatom(toks + *i, BRACEOP)) bracedep++;
      if(isatom(toks + *i, BRACECL)) bracedep--;
      assert(bracedep >= 0);
      assert(toks[*i].gen.type != NOTOK);

      (*i)++;
    } while(bracedep > 0);

    fd->hi = *i -1; // therefore hi is inclusive, not exclusive
    dcl->fundef = fd;

    free(specs); // end of declaration group
    specs = NULL;

    return dcl;
  }
  else
  {
    dcl->fundef = NULL;
  }

  first = 0; // claim first place
  if(isatom(toks + *i, EQ)) // initializer follows
  {
    (*i)++; // move over =

    // find terminating semicolon or comma
    // very similar to nexttoplevel, but on a token list
    // not used often enough to warrant its own function
    int end = *i, dep = 0;
    while(1)
    {
      if((isatom(toks + end, COMMA) || isatom(toks + end, SEMICOLON)) && dep == 0)
        break;
      // TODO assert dep!!!

      if(isatom(toks + end, PARENOP) || isatom(toks + end, BRACKOP) || isatom(toks + end, BRACEOP) || isatom(toks + end, QUESTION))
        dep++;
      if(isatom(toks + end, PARENCL) || isatom(toks + end, BRACKCL) || isatom(toks + end, BRACECL) || isatom(toks + end, COLON))
        dep--;

      assert(dep >= 0);
      assert(toks[end].gen.type != NOTOK);

      end++;
    }

    assert(end != *i);

    link *chain = tokl2ll(toks + *i, end - *i); // turn the initializer into a token list

    struct init *init = parseinit(chain);

    dcl->init = init;
    *i = end; // move over to comma or semicolon
  }
  else // no initializer
  {
    dcl->init = NULL;
  }

  // check what's terminating
  assert(isatom(toks + *i, COMMA) || isatom(toks + *i, SEMICOLON));
  if(isatom(toks + *i, SEMICOLON)) // end of declaration grouping
  {
    free(specs);
    specs = NULL;
    (*i)++;

    // indicate semicolon was reached, if the caller requested to be notified of it
    if(sc)
    {
      *sc = 1;
    }
  }
  else // comma, simply move over
  {
    (*i)++;
  }

  return dcl;

}

//{{{1 linked list tools
int lisin(link *l, int num, int *tokl)
{
  for(int i = 0; i < num; i++)
  {
    if(lisatom(l, tokl[i])) return 1;
  }

  return 0;
}

int lllen(link *l)
{
  if(!l) return 0;
  leftend(l);
  int len = 0;
  while(l)
  {
    len++;
    l = l->right;
  }
  return len;
}

// next top level link that is in atoms
link *nexttoplevel(link *start, int dir, int num, int *atoms)
{
  // read list of tokens
  // va_list ap;
  // token *tokl = malloc(sizeof(token) * num);
  // va_start(ap, num);
  // for(int i = 0; i < num; i++)
  // {
  //   tokl[i] = va_arg(ap, int);
  // }
  // va_end(ap);

  if(!start) return start; // if NULL return NULL

  int dep = 0; // { [ ( ?: depth
  link *curl = start;

  while(1)
  {
    // we do the lisin check on either end of the dep check for when we're searching for (, {, ), }, etc.
    // if(det(curl) && dep == 0) // passes test, return
    if(lisin(curl, num, atoms) && dep == 0)
    {
      return curl;
    }

    if(lisatom(curl, PARENOP) || lisatom(curl, BRACKOP) || lisatom(curl, BRACEOP) || lisatom(curl, QUESTION))
      dep++;
    if(lisatom(curl, PARENCL) || lisatom(curl, BRACKCL) || lisatom(curl, BRACECL) || lisatom(curl, COLON))
      dep--;
    // assert(dep >= 0); // it can go below 0 when going backwards

    // if(det(curl) && dep == 0) // passes test, return
    if(lisin(curl, num, atoms) && dep == 0)
      return curl;

    if(curl == NULL)
      break;
    curl = (dir == RIGHT) ? curl->right : curl->left;
  }

  return NULL;
}


//{{{1 expr manipulation

// create expr of type INT_T containing x for data
expr *makeintexpr(int x)
{
  // create new expression to hold it
  expr *e = makeexpr(PRIM_E, INT_O, 0);
  e->ct = makedt(INT_T);
  e->dat = x;

  return e;
}

void decay(int optype, int argn, expr *e)
{
  assert(e);

  // putctype(e->ct);
  // nline();

  // check if fulfills decay conditions
  if(e->lval) // lvalue
  {
    // array decay to pointer
    if(e->ct && tmis(e->ct, TM_ARR) // is array
                                    // but not operand of these things which require an array
        && optype != SIZEOF_O
        && optype != ADDR_O
        // TODO strlit initializer remains array
      )
    {
      int len = getctlen(e->ct);
      ctype newct = calloc(len, sizeof(typemod));
      memcpy(newct, e->ct, len * sizeof(typemod));

      // turn array into pointer
      newct->gen.type = TM_PTR;
      newct->ptr.isconst = 0;
      newct->ptr.isvolat = 0;

      // replace
      e->ct = newct;

      // no longer lvalue
      e->lval = 0;
    }

    // non-array decay to value
    else if(// not array
            // and not operand of these things which require an lvalue
        optype != SIZEOF_O
        && optype != ADDR_O
        && optype != PREINC_O
        && optype != POSTINC_O
        && optype != PREDEC_O
        && optype != POSTDEC_O
        && !(isasgnop(optype) && argn == 0) // left argument of assignment operator
        )
      // TODO left of .
    {
      // we do no ctype modifying, we just tell 2ASM to get the data out of there
      e->ct = unqual(e->ct); // loses qualifiers
      e->lval = 0; // no longer lvalue
    }
  }

  // function designator
  else if(e->ct && tmis(e->ct, TM_FUNC))
  {
    if(// but not operand of any of these things
        optype != SIZEOF_O
        && optype != ADDR_O)
    {
      // copy into new typemod, offset by 1 to make room for pointer
      int len = getctlen(e->ct);
      ctype newct = calloc(len+1, sizeof(typemod));
      memcpy(newct+1, e->ct, sizeof(typemod)*len);

      // turn function into pointer to function
      newct->gen.type = TM_PTR;
      newct->ptr.isconst = 0;
      newct->ptr.isvolat = 0;

      e->ct = newct;
      e->lval = 0;
    }
  }
  // otherwise not lvalue, carry on

}

/*expr *decay(int optype, expr *e)
{
  // check if fulfills decay conditions
  if(e->lval) // lvalue
  {
    // array decay to pointer
    if(e->ct && tmis(e->ct, TM_ARR) // is array
                                    // but not operand of these things which require an array
        && optype != SIZEOF_O
        && optype != ADDR_O
        // TODO strlit initializer remains array
      )
    {
      // copy into new typemod,
      int len = getctlen(e->ct);
      ctype newct = calloc(len, sizeof(typemod));
      memcpy(newct, e->ct, len);

      // turn array into pointer
      newct->gen.type == TM_PTR;
      newct->ptr.isconst = 0;
      newct->ptr.isvolat = 0;

      // dece (decay expression) wraps around e, using newct as front
      expr *dece = calloc(1, sizeof(expr));
      dece->ct = newct;
      etypeadd(dece, DECAY);
      dece->optype = ARR2PTR_O;
      dece->numargs = 1;

      // dece->args = &e;
      dece->args = malloc(sizeof(expr *));
      dece->args[0] = e;

      dece->lval = 0;

      e = dece; // pass on to newe
    }

    // non-array decay to value
    else if(// not array
            // and not operand of these things which require an lvalue
        optype != SIZEOF_O
        && optype != ADDR_O
        && optype != PREINC_O
        && optype != POSTINC_O
        && optype != PREDEC_O
        && optype != POSTDEC_O)
      // TODO left of .
    {
      // we do no ctype modifying, we just tell 2ASM to get the data out of there
      expr *dece = calloc(1, sizeof(expr));
      dece->ct = unqual(e->ct); // loses qualifiers
      etypeadd(dece, DECAY);
      dece->optype = LVAL2DAT_O;
      dece->numargs = 1;

      // dece->args = &e;
      dece->args = malloc(sizeof(expr*));
      dece->args[0] = e;

      dece->lval = 0; // no longer lvalue

      e = dece;
    }
  }

  // function designator
  else if(e->ct && tmis(e->ct, TM_FUNC))
  {
    if(// but not operand of any of these things
        optype != SIZEOF_O
        && optype != ADDR_O)
    {
      // copy into new typemod, offset by 1 to make room for pointer
      int len = getctlen(e->ct);
      ctype newct = calloc(len+1, sizeof(typemod));
      memcpy(newct+1, e->ct, len);

      // turn function into pointer to function
      newct->gen.type == TM_PTR;
      newct->ptr.isconst = 0;
      newct->ptr.isvolat = 0;

      // dece (decay expression) wraps around e, using newct as front
      expr *dece = calloc(1, sizeof(expr));
      dece->ct = newct;
      etypeadd(dece, DECAY);
      dece->optype = FUN2PTR_O;
      dece->numargs = 1;

      // dece->args = &e;
      dece->args = malloc(sizeof(expr*));
      dece->args[0] = e;

      dece->lval = 0;

      e = dece;
    }
  }
  // otherwise not lvalue, carry on

  return e;
}*/

expr *makeexpr(int type, int optype, int numargs, ...)
{
  expr *newe = calloc(1, sizeof(expr));
  etypeadd(newe, type);
  newe->optype = optype;
  // putd(optype);
  newe->numargs = numargs;

  // if numargs == 0 then don't do this
  if(numargs)
  {
    newe->args = malloc(sizeof(expr *) * numargs);
    va_list ap;
    va_start(ap, numargs);

    expr *e;
    // run through the args
    for(int i = 0; i < numargs; i++)
    {
      e = va_arg(ap, expr *);
      
      // certain types trigger decays, which are prevented by certain optypes
      // puts("calling decay");
      // printf("%p\n", e->label);
      // if(e->label)
      //   puts(e->label);
      // putctype(e->ct);
      // nline();
      decay(optype, i, e);

      // putctype(e->ct);
      // nline();
      
      
      newe->args[i] = e;
    }
    va_end(ap);
  }

  return newe;
}

//{{{1 type checking tools
int isconst(ctype ct)
{
  return ct->gen.type == TM_DAT && ct->dat.isconst
    || ct->gen.type == TM_PTR && ct->ptr.isconst;
}

int isvolat(ctype ct)
{
  return ct->gen.type == TM_DAT && ct->dat.isvolat
    || ct->gen.type == TM_PTR && ct->ptr.isvolat;
}

// no const pointers, no const data, no arrays, functions, etc.
int ismodifiable(ctype ct)
{
  // we only look at the first in the typemod list

  // incomplete type
  if(incomplete(ct))
    return 0;
  
  // direct data, must not be qualified with const
  if(ct->gen.type == TM_DAT)
  {
    return !ct->dat.isconst;
  }

  // pointer, must not be const pointer
  if(ct->gen.type == TM_PTR)
  {
    return !ct->ptr.isconst;
  }
  
  if(ct->gen.type == TM_ARR)
    return 0;

  if(ct->gen.type == TM_FUNC)
    return 0;

  return 1;
  // TODO structs, unions with const members, incomplete types
}

// get number of typemods in ctype
int getctlen(ctype ct)
{
  int len = 1;
  while(ct->gen.type != TM_DAT)
  {
    len++;
    ct++;
  }

  return len;
}

// remove qualifiers from type
ctype unqual(ctype ct)
{
  // create new ct and copy over
  int len = getctlen(ct);
  ctype newct = malloc(len * sizeof(typemod));
  memcpy(newct, ct, sizeof(typemod) * len);

  // remove any existing quals from first typemod
  if(newct->gen.type == TM_PTR)
  {
    newct->ptr.isconst = 0;
    newct->ptr.isvolat = 0;
  }

  else if(newct->gen.type == TM_DAT)
  {
    newct->dat.isconst = 0;
    newct->dat.isvolat = 0;
  }

  return newct;
}

int qualcmp(int c1, int c2, int v1, int v2, int mode)
{
  if(mode == QM_NOCARE)
    return 1;

  if(mode == QM_SUPERSET)
    return c1 >= c2
      && v1 >= v2;

  if(mode == QM_STRICT)
    return c1 == c2
      && v1 == v2;

  throw("invalid qualmode");
}

// check for incomplete types
int incomplete(ctype ct)
{
  if(ctisdt(ct, VOID_T))
    return 1;

  if(ct->gen.type == TM_PTR)
    return 0;

  if(ct->gen.type == TM_ARR && ct->arr.len == -1)
    return 1;

  return 0;
}

int isobject(ctype ct)
{
  return (!incomplete(ct)) && (!tmis(ct, TM_FUNC));
}

// return 1 if toplevel is qualified ptr or dat
int isqual(ctype ct)
{
  if(ct->gen.type == TM_PTR)
  {
    return ct->ptr.isconst || ct->ptr.isvolat;
  }
  if(ct->gen.type == TM_DAT)
  {
    return ct->dat.isconst || ct->dat.isvolat;
  }
  return 0;
}

// disallows array of incomplete type, function returning bad type, etc.
int validct(ctype ct)
{
  int len = getctlen(ct);
  for(int i = 0; i < len-1; i++)
  {
    // array of incomplete
    if(ct[i].gen.type == TM_ARR)
    {
      if(incomplete(ct+i+1))
        return 0;
    }

    // function returning qualified type
    if(ct[i].gen.type == TM_FUNC)
    {
      // no returning qualified type
      if(isqual(ct+i+1))
        return 0;

      // no returning array
      if(ct[i+1].gen.type == TM_ARR)
        return 0;

      // no returning function
      if(ct[i+1].gen.type == TM_FUNC)
        return 0;
    }
  }

  return 1;
}

ctype makecompos(ctype ct1, ctype ct2, int qualmode)
{
  // regardless of qualmode, the resulting quals are the union of the input quals
  // qualmode is just here for the assert
  assert(iscompat(ct1, ct2, qualmode));

  // create new ctype
  int len = getctlen(ct1);
  ctype newct = malloc(sizeof(typemod) * len);

  for(int i = 0; i < len; i++)
  {
    int type = ct1[i].gen.type;
    newct[i].gen.type = type;
    
    if(type == TM_PTR)
    {
      newct[i].ptr.isconst = ct1[i].ptr.isconst || ct2[i].ptr.isconst;
      newct[i].ptr.isvolat = ct1[i].ptr.isvolat || ct2[i].ptr.isvolat;
    }

    else if(type == TM_ARR)
    {
      newct[i].arr.len = max(ct1[i].arr.len, ct2[i].arr.len);
    }

    else if(type == TM_FUNC)
    {
      int np1 = ct1[i].func.np;
      int np2 = ct2[i].func.np;

      if(np1 == -1 && np2 == -1) // leave params unspecified
      {
        newct[i].func.params = NULL;
        newct[i].func.np = -1;
      }

      else
      {
        newct[i].func.np = max(np1, np2);

        if(np1 == -1)
          newct[i].func.params = ct2[i].func.params;
        else if(np2 == -1)
          newct[i].func.params = ct1[i].func.params;
        else
        {
          decl *params = calloc(np1, sizeof(decl));

          for(int j = 0; j < np1; j++)
          {
            params[j].ct = makecompos(ct1[i].func.params[j].ct, ct1[i].func.params[j].ct, QM_NOCARE);
          }

          newct[i].func.params = params;
        }
      }
    }

    else if(type == TM_DAT)
    {
      newct[i].dat.isconst = ct1[i].dat.isconst || ct2[i].dat.isconst;
      newct[i].dat.isvolat = ct1[i].dat.isvolat || ct2[i].dat.isvolat;
      newct[i].dat.dt = ct1[i].dat.dt;
    }
  }

  return newct;
}

// types are compatible for some purpose (assignment, function params, multiple declarations)
// purpose determined by qualmode (how qual compatibility is determined)
int iscompat(ctype ct1, ctype ct2, int qualmode)
{
  int t1 = ct1->gen.type;
  int t2 = ct2->gen.type;

  if(t1 != t2) // typemods differ, not allowed; this includes the case where one ends before the other (TM_DAT and something else)
    return 0;

  else if(t1 == TM_DAT && t2 == TM_DAT) // compare dattypes and quals
  {
    if(ct1->dat.dt != ct2->dat.dt)
      return 0;
    
    return qualcmp(ct1->dat.isconst, ct2->dat.isconst,
        ct1->dat.isvolat, ct1->dat.isvolat,
        qualmode);
  }

  else if(t1 == TM_ARR)
  {
    // both types must not be incomplete
    if(ct1->arr.len != -1 && ct2->arr.len != -1)
    {
      if(ct1->arr.len != ct2->arr.len)
        return 0;
    }
    
    // if incomplete type, assume compatible
    // recurse
    return iscompat(ct1+1, ct2+1, qualmode);
  }

  else if(t1 == TM_PTR)
  {
    // check that quals are compatible
    if(!qualcmp(ct1->ptr.isconst, ct2->ptr.isconst,
          ct1->ptr.isvolat, ct2->ptr.isvolat,
          qualmode))
      return 0;

    // recurse
    // return supersedes(ct1+1, ct2+1);
    return iscompat(ct1+1, ct2+1, qualmode);
  }

  else if(t1 == TM_FUNC)
  {
    decl *p1 = ct1->func.params;
    decl *p2 = ct2->func.params;

    if(p1 && p2)
    {
      if(ct1->func.np != ct2->func.np) // same length?
        return 0;
      
      // decl *pl1 = (decl *) p1->cont;
      // decl *pl2 = (decl *) p2->cont;

      // recursively check parameters for compatibility
      for(int i = 0; i < ct1->func.np; i++)
      {
        // params are decls, because of possible identifiers. we ignore that and only look at the ct
        if(!iscompat(p1[i].ct, p2[i].ct, QM_NOCARE)) // change qualmode to nocare, because we do not care
          return 0;
      }
    }
    // if one or the other is unspecified, then we let it through

    // recurse
    return iscompat(ct1+1, ct2+1, qualmode);
  }
  else
  {
    throw("impossible!");
    return 0;
  }
}

int isptr(ctype ct)
{
  return ct->gen.type == TM_PTR;
}

int tmis(typemod *tm, int type)
{
  return tm->gen.type == type;
}

expr *makecast(ctype ct, expr *e)
{
  expr *newe = makeexpr(CAST_E, CAST_O, 1, e);
  newe->ct = ct;
  
  return newe;
}

// cast one dt to another (both original and result must be dt
expr *makedtcast(int dt, expr *e)
{
  ctype ct = e->ct;
  assert(ct->gen.type == TM_DAT);

  if(ct->dat.dt == dt) // redundant cast
  {
    return e;
  }

  // copy over isconst, isvolat, type
  ctype newct = malloc(sizeof(typemod));
  memcpy(newct, ct, sizeof(typemod));

  newct->dat.dt = dt;

  expr *newe = makecast(newct, e);
  return newe;
}

// perform integral promotion
// wraps expression in cast if promotion necessary
expr *intprom(expr *e)
{
  ctype ct = e->ct;
  // if(tmis(ct, TM_DAT)) // must be direct data to be integral

  // must be integral
  // assert(isintegral(ct));
  // we leave it to other functions to check this because eu.x. unary + - do integral promotion on arithmetic type, which could be floating

  int dt = ct->dat.dt;

  // convert to int
  if(dt == CHAR_T
      || dt == UCHAR_T
      || dt == SINT_T)
  {
    // ctype newct = malloc(sizeof(typemod));
    // memcpy(newct, ct, sizeof(typemod)); // copy over isconst, isvolat, type
    // newct->gen.dt = INT_T; // convert

    // // do implicit cast
    // expr *newe = makecast(newct, e);
    // return newe;

    expr *newe = makedtcast(INT_T, e);
    return newe;
  }

  // convert to unsigned int
  else if(dt == USINT_T)
  {
    // ctype newct = malloc(sizeof(typemod));
    // memcpy(newct, ct, sizeof(typemod));
    // newct->gen.dt = UINT_T;

    // expr *newe = makecast(newct, e);
    // return newe;

    expr *newe = makedtcast(UINT_T, e);
    return newe;
  }

  // no integral promotion, return unchanged
  return e;
}

// expr is typemod (top level)
int eistm(expr *e, int t)
{
  return e->ct->gen.type == t;
}

// returns a typemod list of one element which is a dt
ctype makedt(int dt)
{
  ctype ct = calloc(1, sizeof(typemod));
  ct->gen.type = TM_DAT;
  ct->dat.dt = dt;
  return ct;
}

// expr is dattype
int eisdt(expr *e, int dt)
{
  assert(e);
  return tmis(e->ct, TM_DAT) && e->ct->dat.dt == dt;
}

int ctisdt(ctype ct, int dt)
{
  assert(ct);
  return ct->gen.type == TM_DAT && ct->dat.dt == dt;
}

// if one is dt, make other dt too
// only used in usualarith()
int makesametype(expr **e1, expr **e2, int dt)
{
  if(eisdt(*e1, dt))
  {
    *e2 = makedtcast(dt, *e2);
    return 1;
  }
  else if(eisdt(*e2, dt))
  {
    *e1 = makedtcast(dt, *e1);
    return 1;
  }

  // did not convert, neither expr is type dt
  return 0;
}

// perform usual arithmetic conversions on two arithmetic types
// (bring them to a common compatible type)
void usualarith(expr **e1, expr **e2)
{
  assert(isarith((*e1)->ct));
  assert(isarith((*e2)->ct));

  // floating conversions
  if(makesametype(e1, e2, LDUB_T)) ; // long double
  else if(makesametype(e1, e2, DUB_T)) ; // double
  else if(makesametype(e1, e2, FLOAT_T)) ; // float

  // integral conversions
  else
  {
    // perform integral promotion on both operands
    *e1 = intprom(*e1);
    *e2 = intprom(*e2);

    if(makesametype(e1, e2, ULINT_T)) ; // unsigned long int

    // if one is long int and other is unsigned int
    else if(eisdt(*e1, LINT_T) && eisdt(*e2, UINT_T)
        || eisdt(*e1, UINT_T) && eisdt(*e2, LINT_T))
    {
      // convert both to unsigned long int
      *e1 = makedtcast(ULINT_T, *e1);
      *e2 = makedtcast(ULINT_T, *e2);
    }

    else if(makesametype(e1, e2, LINT_T)) ; // long int

    else if(makesametype(e1, e2, UINT_T)) ; // unsigned int

    else
    {
      // it should be logically impossible for this to fail, because of the integral promotion above
      assert(eisdt(*e1, INT_T));
      assert(eisdt(*e2, INT_T));
    }
  }

}

//{{{1 expression parser

// REQUIREMENTS
// start can be anywhere in the linked list
// use rightend() and leftend() to get it where you want
// the left and right ends must be null terminated

expr *parseexpr(link *start)
{
  // assert(start);
  testerr(start, "parseexpr: null start");

  rightend(start);
  static int commaops[] = {COMMA};
  link *comma = nexttoplevel(start, LEFT, 1, commaops);
  // assert(start != comma); // no empty subexprs
  testerr(start != comma, "parseexpr: empty comma subexr"); // no empty subexprs
  
  if(!comma) // base case, drop down
  {
    return parseasgnexpr(start);
  }
  
  // recurse sideways
  
  // detach
  start->right = NULL; // can't hurt
  // comma->right->left = NULL;
  // comma->left->right = NULL;
  // putd(1);
  // here();
  sever(comma);

  expr *e1 = parseexpr(comma->left);
  if(!e1) return NULL;
  expr *e2 = parseasgnexpr(start);
  if(!e2) return NULL;

  expr *newe = makeexpr(EXPR, COMMA_O, 2, e1, e2);
  e1 = newe->args[0];
  e2 = newe->args[1];


  assert(e2->ct);
  newe->ct = e2->ct; // inherits type from right operand
  newe->lval = e2->lval; // inherits lvalue status

  return newe;
}

int asgnops[] = {EQ, TIMESEQ, DIVEQ, MODEQ, PLUSEQ, MINEQ, SHLEQ, SHREQ, ANDEQ, XOREQ, OREQ};

int atomisasgnop(int x)
{
  for(int i = 0; i < sizeof(asgnops)/sizeof(int); i++)
  {
    if(asgnops[i] == x) return 1;
  }
  return 0;
}

int isasgnop(enum optype o)
{
  return o == EQ_O
    || o == TIMESEQ_O
    || o == DIVEQ_O
    || o == MODEQ_O
    || o == PLUSEQ_O
    || o == MINEQ_O
    || o == SHLEQ_O
    || o == SHREQ_O
    || o == ANDEQ_O
    || o == XOREQ_O
    || o == OREQ_O;
}

// check if ct1 = ct2 is a valid assignment expression
void checkasgncompat(enum optype optype, ctype ct1, ctype ct2)
{
  // operation-agnostic checks
  if(isarith(ct1) && isarith(ct2)) ; // both arithmetic

  //else if(TODO: structures and unions);

  else if(tmis(ct1, TM_PTR) && tmis(ct2, TM_PTR)
      && ((tmis(ct1+1, TM_DAT) && ct1[1].dat.dt == VOID_T)
        || (tmis(ct2+1, TM_DAT) && ct2[1].dat.dt == VOID_T))) // one is pointer to any, other is pointer to void
  {
    // make sure left hand pointed-to quals are stricter than right
    if(isconst(ct2+1)) assert(isconst(ct1+1));
    if(isvolat(ct2+1)) assert(isvolat(ct1+1));
  }

  //else if(TODO: lhs ptr, rhs constant 0);

  else if(tmis(ct1, TM_PTR) && tmis(ct2, TM_PTR)) // both pointers to functions or objects
  {
    // check compatibility, using superset qualmode
    // assert(iscompat(ct1+1, ct2+1, QM_SUPERSET));
    if(!iscompat(ct1+1, ct2+1, QM_SUPERSET))
    {
      putctype(ct1);
      nline();
      putctype(ct2);
      nline();
      throw("not compatible");
    }
  }

  else
  {
    // fprintf(stderr, "%p %p\n", ct1, ct2);
    // putctype(ct1);
    // nline();
    // putctype(ct2);
    // nline();
    // printf("%d %d\n", ct1->gen.type, ct2->gen.type);
    // printf("%d %d\n", ct1->dat.dt, ct2->dat.dt);
    // putd(isarith(ct1));
    throw("invalid assignment types");
  }

  
  // operation-specific checks
  if(optype == EQ_O) ; // no extra constraints for =
  else if(optype == PLUSEQ_O || optype == MINEQ_O)
  {
    // if += or -=, pointer on left implies integral on right
    if(ct1->gen.type == TM_PTR)
    {
      assert(isintegral(ct2));
    }
  }

  else // all other operators
  {
    // TODO type consistent with those allowed by the corresponding binary operator
    putd(optype);
    puts(hropt[optype]);
    throw("other assignment types not supported yet");
  }
  
  
}

expr *parseasgnexpr(link *start)
{
  // assert(start);
  testerr(start, "parseasgnexpr: empty start");
  leftend(start);

  static int ops[100] = {
    [EQ]=EQ_O,
  [TIMESEQ]=TIMESEQ_O,
  [DIVEQ]=DIVEQ_O,
  [MODEQ]=MODEQ_O,
  [PLUSEQ]=PLUSEQ_O,
  [MINEQ]=MINEQ_O,
  [SHLEQ]=SHLEQ_O,
  [SHREQ]=SHREQ_O,
  [ANDEQ]=ANDEQ_O,
  [XOREQ]=XOREQ_O,
  [OREQ]=OREQ_O,};


  link *op = nexttoplevel(start, RIGHT, sizeof(asgnops)/sizeof(int), asgnops);
  // assert(start != op); // no empty assignment
  testerr(start != op, "parseasgnexpr: empty assignment"); // no empty assignment
  
  if(!op)
  {
    return parsecondexpr(start);
  }
  
  start->left = NULL;
  // op->right->left = NULL;
  // op->left->right = NULL;

  // putd(2);
  // here();
  sever(op);

  expr *e1 = parseunaryexpr(start);
  if(!e1) return NULL;
  expr *e2 = parseasgnexpr(op->right);
  if(!e2) return NULL;

  int optype = ops[op->cont.tok->atom.cont];

  // create new expression
  expr *newe = makeexpr(ASGN_E, optype, 2, e1, e2);
  e1 = newe->args[0];
  e2 = newe->args[1];

  // perform type checks
  assert(e1->lval); // LHS must be lvalue
  assert(e1->ct);
  assert(ismodifiable(e1->ct)); // modifiable type, including ban on const qual, etc.
  // assert(!e1->ct->isconst);
  // if(e1->ct->tms)
  // {
  //   assert(e1->ct->tms[0].gen.type != TM_ARR); // no arrays
  // }

  ctype ct1 = e1->ct;
  ctype ct2 = e2->ct;

  // one of the following has to be true

  // we throw the below logic into a function so that FUN_O can use it to check for arg->param compatibility
  // fputs("hi\n", stderr);
  checkasgncompat(optype, ct1, ct2);
  

  // TODO see C90 6.3.16.2 for rules about +=, -=, etc. with pointers
  // TODO remember no double evaluation for compound assignments

  
  // if(optype == EQ_O) ; // no extra constraints for =
  // if(optype == PLUSEQ_O || optype == MINEQ_O)
  // {
  //   // if += or -=, pointer on left implies integral on right
  //   if(ct1->gen.type == TM_PTR)
  //   {
  //     assert(isintegral(ct2));
  //   }
  // }

  // else // all other operators
  // {
  //   // TODO type consistent with those allowed by the corresponding binary operator
  //   throw("other assignment types not supported yet");
  // }

  // make implicit cast of right side to left type
  newe->ct = unqual(e1->ct); // unqualified copy of left hand ct
  if(!iscompat(newe->ct, newe->args[1]->ct, QM_STRICT))
  {
    newe->args[1] = makecast(newe->ct, newe->args[1]);
  }

  newe->lval = 0;


  return newe;
}

expr *parsecondexpr(link *start)
{
  // assert(start);
  testerr(start, "parsecondexpr: null start");
  leftend(start);

  int ql[] = {QUESTION};
  int cl[] = {COLON};

  link *quest = nexttoplevel(start, RIGHT, 1, ql);

  // assert(start != quest);
  testerr(start != quest, "parsecondexpr: empty question");

  if(!quest)
    return parselorexpr(start);

  // puts("-------------------");
  // putd(1);
  link *colon = findmatch(quest, RIGHT, QUESTION, COLON);
  // putd(2);
  // assert(quest + 1 != colon);
  testerr(quest + 1 != colon, "parsecondexpr: empty left branch");
  // putd(3);
  // here();
  sever(quest);
  // here();
  sever(colon);

  expr *e1 = parselorexpr(quest->left);
  if(!e1) return NULL;
  expr *e2 = parseexpr(quest->right);
  if(!e2) return NULL;
  expr *e3 = parsecondexpr(colon->right);
  if(!e3) return NULL;

  expr *newe = makeexpr(COND_E, TERN_O, 3, e1, e2, e3);
  e1 = newe->args[0];
  e2 = newe->args[1];
  e3 = newe->args[2];

  // type checks/conversions

  assert(isscalar(e1->ct));
  ctype newct = NULL;
  
  // two arithmetic types: perform UAC
  if(isarith(e2->ct) && isarith(e3->ct))
  {
    usualarith(&e2, &e3);
    newct = e2->ct;
  }

  else if(eisdt(e2, VOID_T) && eisdt(e3, VOID_T)) ; // both void: do nothing
  
  else if(eistm(e2, TM_PTR) && eistm(e3, TM_PTR)
      && iscompat(e2->ct+1, e3->ct+1, QM_NOCARE)) // pointers to compatible types
  {
    newct = makecompos(e2->ct, e3->ct, QM_NOCARE);
  }

  // TODO one is pointer, one is nullptr constant

  // otherwise must be pointers
  else if(eistm(e2, TM_PTR) && eistm(e3, TM_PTR))
  {
    // one is pointer, other is pointer to void
    // then convert the other one to pointer to void as well

    if(ctisdt(e2->ct+1, VOID_T))
    {
      e3 = makecast(e2->ct, e3);
    }
    else if(ctisdt(e3->ct+1, VOID_T))
    {
      e2 = makecast(e3->ct, e2);
    }
    else
    {
      throw("invalid pointer targets in parecondexpr()");
    }

    newct = e2->ct;
  }
    // && (eisdt(e2+1, VOID_T) && eisdt(e3+1, VOID_T)))
    // {

  else
  {
    throw("invalid types for operands of parsecondexpr()");
  }

  // TODO there is a sequence point after evaluation of e1. not sure if i need to take this into account at all

  newe->ct = newct;
  newe->lval = 0; // footnote on C90 standard page 52
  return newe;
}

// parse LTR binary expression, a very common expression type, which can be generalized
expr * parseltrbinexpr(link *start, int etype, int num, int *atoms, int *optypes, expr *(*down)(link *), expr *(*side)(link *))
{
  // assert(start);
  testerr(start, "parseltrbinexpr: empty start");
  rightend(start);

  /*
    to deal with ambiguous unary/binary operators:
    we find the first atom in atoms[], and try parsing the left and right branches of that
    if that doesn't work, we assume it's a unary operator, and find the next atom in atoms[]
    if all fail, then we assume all are unary, and move down
    the testerr() system should take care of invalid expressions
    this way expressions such as
      a + - b
    work out
  */

  // backup linked list so we can restore later
  int len = lllen(start);
  link **backup = malloc(sizeof(link *)*(len + 2)); // number of links + 2 for NULL on each end
  // putd(len);
  link *temp = start;
  leftend(temp);
  for(int i = 1; i <= len; i++)
  {
    // puts("backing up");
    // puttok(*temp->cont.tok);
    // nline();nline();
    backup[i] = temp;
    temp = temp->right;
  }
  backup[0] = NULL;
  backup[len+1] = NULL;

  link *op;
  link *curl = start;
  while(1)
  {
    // puts("hi");
    op = nexttoplevel(curl, LEFT, num, atoms);
    if(!op) // we've reached the far left side, move down
      return down(start);

    // puttok(*op->cont.tok);
    testerr(start != op, "parseltrbinexpr: empty right arg"); // both unary and binary require a right arg

    // get optype from atom
    int atom = op->cont.tok->atom.cont;
    int optype = corresp(num, atoms, optypes, atom);

    // sever, but don't give error if NULL on either side
    trysever(op);

    // puts("about to call");
    // putd(op->left);
    // expr *e1 = parseltrbinexpr(op->left, etype, num, atoms, optypes, down); // recurse sideways

    // recurse sideways
    expr *e1 = side(op->left);

    if(!e1) // parsing left branch failed
    {
      // putd(op);
      // puttok(*op->cont.tok);
      testerr(lisin(op, cbulen, canbeunary), "parseltrbinexpr: binary operator trying to be unary"); // make sure it can be a unary operator
      
      curl = op->left; // move over the op we just tried
      // unsever using backup[]
      for(int i = 1; i <= len; i++)
      {
        backup[i]->left = backup[i-1];
        backup[i]->right = backup[i+1];
      }

      continue;
    }

    free(backup);
    expr *e2 = down(start);
    if(!e2) return NULL;

    expr *newe = makeexpr(etype, optype, 2, e1, e2);
    return newe;
  }


//   // assert(start != op); // both unary and binary require a right arg
//   // if(!op->left) // no left arg; it's in unary form
//   // {
//   //   // assert(canbeunary[op->cont.tok->atom.cont]);
//   //   testerr(canbeunary[op->cont.tok->atom.cont], "parseltrbinexpr: empty left arg");
//   //   return down(start);
//   // }
//   // puts("iuieiei");


//   // start->right = NULL;

//   expr *e1 = parseltrbinexpr(op->left, etype, num, atoms, optypes, down); // recurse sideways

//   expr *e2 = down(start);
  
//   expr *newe = makeexpr(etype, optype, 2, e1, e2);
//   return newe;
}

expr *parselorexpr(link *start)
{
  // here();
  static int at[] = {LOGOR};
  static int op[] = {LOR_O};

  expr *newe = parseltrbinexpr(start, LOR_E, 1, at, op, parselandexpr, parselorexpr);
  // if(!eistype(newe, LOR_E))
  //   return newe;
  checkours(newe, LOR_E);
  
  newe->ct = makedt(INT_T); // type is always int
  return newe;
}

expr *parselandexpr(link *start)
{
  // here();
  static int at[] = {LOGAND};
  static int op[] = {LAND_O};
  expr *newe = parseltrbinexpr(start, LAND_E, 1, at, op, parseorexpr, parselandexpr);
  // if(!eistype(newe, LAND_E))
  //   return newe;
  checkours(newe, LAND_E);
  
  newe->ct = makedt(INT_T);
  return newe;
}

expr *parseorexpr(link *start)
{
  // here();
  static int at[] = {BITOR};
  static int op[] = {BOR_O};

  expr *newe = parseltrbinexpr(start, OR_E, 1, at, op, parsexorexpr, parseorexpr);
  // if(!eistype(newe, OR_E))
  //   return newe;
  checkours(newe, OR_E);
  
  
  // perform UAC on args
  usualarith(&newe->args[0], &newe->args[1]);

  // copy type of parents
  newe->ct = newe->args[0]->ct;

  return newe;
}

expr *parsexorexpr(link *start)
{
  // here();
  static int at[] = {BITXOR};
  static int op[] = {XOR_O};

  expr *newe = parseltrbinexpr(start, XOR_E, 1, at, op, parseandexpr, parsexorexpr);
  // if(!eistype(newe, XOR_E))
  //   return newe;
  checkours(newe, XOR_E);
  
  usualarith(&newe->args[0], &newe->args[1]);

  newe->ct = newe->args[0]->ct;

  return newe;
}

expr *parseandexpr(link *start)
{
  // here();
  static int at[] = {BITAND};
  static int op[] = {BAND_O};
  expr *newe = parseltrbinexpr(start, AND_E, 1, at, op, parseeqexpr, parseandexpr);
  // if(!eistype(newe, AND_E))
  //   return newe;
  checkours(newe, AND_E);
  
  usualarith(&newe->args[0], &newe->args[1]);

  newe->ct = newe->args[0]->ct;

  return newe;
}

expr *parseeqexpr(link *start)
{
  // here();
  static int at[] = {EQEQ, NOTEQ};
  static int op[] = {EQEQ_O, NEQ_O};
  expr *newe = parseltrbinexpr(start, EQUAL_E, 2, at, op, parserelexpr, parseeqexpr);
  // if(!eistype(newe, EQUAL_E))
  //   return newe;
  checkours(newe, EQUAL_E);
  

  newe->ct = makedt(INT_T);

  ctype *ct1 = &newe->args[0]->ct;
  ctype *ct2 = &newe->args[1]->ct;

  // both arithmetic
  if(isarith(*ct1) && isarith(*ct2))
  {
    usualarith(&newe->args[0], &newe->args[1]);
  }

  // pointers to compatible types
  else if((*ct1)->gen.type == TM_PTR && (*ct2)->gen.type == TM_PTR && iscompat(*ct1+1, *ct2+1, QM_NOCARE));

  else if((*ct1)->gen.type == TM_PTR && ctisdt(*ct1+1, VOID_T) && (*ct2)->gen.type == TM_PTR)
  {
    // cast non void* to void*
    newe->args[1] = makecast(*ct1, newe->args[1]);
  }
  else if((*ct2)->gen.type == TM_PTR && ctisdt(*ct2+1, VOID_T) && (*ct1)->gen.type == TM_PTR)
  {
    newe->args[0] = makecast(*ct2, newe->args[0]);
  }

  // TODO pointer and nullptr constant
  else
  {
    throw("parseeqexpr: bad types");
  }

  return newe;
}

expr *parserelexpr(link *start)
{
  // here();
  static int at[] = {LESS, GREAT, LEQ, GEQ};
  static int op[] = {LT_O, GT_O, LEQ_O, GEQ_O};
  expr *newe = parseltrbinexpr(start, RELAT_E, 4, at, op, parseshiftexpr, parserelexpr);
  // if(!eistype(newe, RELAT_E))
  //   return newe;
  checkours(newe, RELAT_E);
  

  newe->ct = makedt(INT_T);

  ctype *ct1 = &newe->args[0]->ct;
  ctype *ct2 = &newe->args[1]->ct;

  // both arithmetic
  if(isarith(*ct1) && isarith(*ct2))
  {
    usualarith(&newe->args[0], &newe->args[1]);
  }

  // pointers to compatible types
  else if((*ct1)->gen.type == TM_PTR && (*ct2)->gen.type == TM_PTR && iscompat(*ct1+1, *ct2+1, QM_NOCARE));

  else
  {
    throw("parserelexpr: bad types");
  }

  return newe;
}

expr *parseshiftexpr(link *start)
{
  // here();
  static int at[] = {SHL, SHR};
  static int op[] = {SHL_O, SHR_O};
  expr *newe = parseltrbinexpr(start, SHIFT_E, 2, at, op, parseaddexpr, parseshiftexpr);
  // if(!eistype(newe, SHIFT_E))
  //   return newe;
  checkours(newe, SHIFT_E);
  

  // both integral
  assert(isintegral(newe->args[0]->ct));
  assert(isintegral(newe->args[1]->ct));

  // perform integral promotions
  newe->args[0] = intprom(newe->args[0]);
  newe->args[1] = intprom(newe->args[1]);

  // inherit type from left promoted arg
  newe->ct = newe->args[0]->ct;

  return newe;
}

expr *parseaddexpr(link *start)
{
  // here();
  static int at[] = {PLUS, MIN};
  static int op[] = {ADD_O, SUB_O};
  expr *newe = parseltrbinexpr(start, ADD_E, 2, at, op, parsemultexpr, parseaddexpr);

  // if(!eistype(newe, ADD_E))
  //   return newe;
  checkours(newe, ADD_E);

  ctype ct1 = newe->args[0]->ct;
  ctype ct2 = newe->args[1]->ct;


  // subtraction:
  if(newe->optype == SUB_O)
  {
    // both arithmetic
    if(isarith(ct1) && isarith(ct2))
    {
      usualarith(&newe->args[0], &newe->args[1]);

      newe->ct = newe->args[0]->ct;
    }

    // both pointer to compat object
    else if(isptr(ct1) && isptr(ct2) && iscompat(ct1+1, ct2+1, QM_NOCARE))
    {
      // must be objects, not incomplete
      assert(!incomplete(ct1+1));
      assert(!incomplete(ct2+1));

      newe->ct = makedt(INT_T); // no ptrdiff_t in this implementation, just int (32 bits)
    }

    // pointer (to object) minus integral
    else if(isptr(ct1) && isintegral(ct2))
    {
      // must be object, not incomplete
      assert(!incomplete(ct1+1));

      // inherit type of pointer
      newe->ct = ct1;
    }

    else
    {
      throw("parseaddexpr, SUB_O: bad types");
    }
  }

  else if(newe->optype == ADD_O)
  {
    // both arithmetic
    if(isarith(ct1) && isarith(ct2))
    {
      usualarith(&newe->args[0], &newe->args[1]);

      newe->ct = newe->args[0]->ct;
    }

    // one integral, one pointer to object
    else if(isintegral(ct1) && isptr(ct2))
    {
      assert(!incomplete(ct2+1));

      // inherit type of pointer
      newe->ct = ct2;
    }
    else if(isptr(ct1) && isintegral(ct2))
    {
      assert(!incomplete(ct1+1));

      // inherit type of pointer
      newe->ct = ct1;
    }

    else
    {
      throw("parseaddexpr, ADD_O: bad types");
    }
  }

  return newe;
}

expr *parsemultexpr(link *start)
{
  // here();
  static int at[] = {STAR, DIV, MOD};
  static int op[] = {MULT_O, DIV_O, MOD_O};
  expr *newe = parseltrbinexpr(start, MULT_E, 3, at, op, parsecastexpr, parsemultexpr);

  // if(!eistype(newe, MULT_E))
  //   return newe;
  checkours(newe, MULT_E);

  ctype ct1 = newe->args[0]->ct;
  ctype ct2 = newe->args[1]->ct;

  // must both be arithmetic
  assert(isarith(ct1));
  assert(isarith(ct2));
  
  // if mod, must both be integral
  if(newe->optype == MOD_O)
  {
    assert(isintegral(ct1));
    assert(isintegral(ct2));
  }

  // perform UAC
  usualarith(&newe->args[0], &newe->args[1]);

  // inherit type
  newe->ct = newe->args[0]->ct;
  
  return newe;
}

expr *parsecastexpr(link *start)
{
  // here();
  // assert(start);
  testerr(start, "parsecastexpr: empty start");

  // leftend(start);
  // puts("parsecastexpr");
  // puttok(*start->cont.tok);
  // rightend(start);
  // puttok(*start->cont.tok);
  // nline();
  // nline();

  leftend(start);
  // puttok(*start->cont.tok);

  if(lisatom(start, PARENOP) && isdeclspec(*start->right->cont.tok)) // it's a cast
  {
    // puts("hi");
    link *cl = findmatch(start, RIGHT, PARENOP, PARENCL);

    start->right->left = NULL;
    // putd(5);
    // here();
    sever(cl);
    // cl->left->right = NULL;
    
    ctype ct = parsetypename(start->right);
    expr *e = parsecastexpr(cl->right);
    if(!e) return NULL;

    // cast type must be scalar type or void
    assert(ctisdt(ct, VOID_T) || isscalar(ct));

    // cast target must be scalar
    assert(isscalar(e->ct));
    
    expr *newe = makecast(ct, e);
    return newe;
  }

  else // unary expr
  {
    // here();
    // puttok(*start->cont.tok);
    // putd(start->left);
    // putd(start->right);
    // nline();nline();
    expr *e = parseunaryexpr(start);
    // putd(e);
    return e;
  }
}

expr *parseunaryexpr(link *start)
{
  // here();
  // assert(start);
  testerr(start, "parseunaryexpr: empty start");
  leftend(start);
  // here();
  // putd(start);
  // puttok(*start->cont.tok);

  int optype = -1;

  if(lisatom(start, INC)) optype = PREINC_O;
  else if(lisatom(start, DEC)) optype = PREDEC_O;

  // unary operators
  else if(lisatom(start, BITAND)) optype = ADDR_O;
  else if(lisatom(start, STAR)) optype = POINT_O;
  else if(lisatom(start, PLUS)) optype = UPLUS_O;
  else if(lisatom(start, MIN)) optype = UMIN_O;
  else if(lisatom(start, BITNOT)) optype = BNOT_O;
  else if(lisatom(start, LOGNOT)) optype = LNOT_O;

  // &, *, +, -, ~, !
  if(lisunaryop(start))
  {
    // here();
    // putd(start->left);
    // putd(start->right);
    // here();
    testerr(start->right, "parseunaryexpr: no operand following unary operator");
    start->right->left = NULL;
    // here();
    expr *e = parsecastexpr(start->right);
    // testerr(e, "parseunaryexpr: null castexpr below unary op");
    if(!e) return NULL;

    expr *newe = makeexpr(UNAR_E, optype, 1, e);
    e = newe->args[0];

    ctype ct = e->ct;


    // type checking
    // &
    if(optype == ADDR_O)
    {
      // must be one of the following, or error:
      // function designater
      if(tmis(ct, TM_FUNC)) ;
      // lvalue, object, not register
      else if(e->lval && !incomplete(ct)) ;
      // TODO no register storeclass in declaration
      else throw("parseunaryexpr: bad type");

      // newct is pointer to ct
      int len = getctlen(ct);
      ctype newct = calloc(len + 1, sizeof(typemod));
      memcpy(newct + 1, ct, len * sizeof(typemod));
      newct->gen.type == TM_PTR;

      newe->ct = newct;
    }

    // *
    else if(optype == POINT_O)
    {
      if(!isptr(ct))
      {
        putctype(ct);
        nline();
        throw("not pointer");
      }
      // assert(isptr(ct));
      // LEH something's making it a char instead of char *
      
      // assert(tmis(ct+1, TM_FUNC) || incomplete(ct+1));
      
      // result is lvalue, if not func
      
      // if not pointing to function and not incomplete (object), then lvalue
      if(!tmis(ct+1, TM_FUNC) && !incomplete(ct+1))
      {
        newe->lval = 1;
      }
      newe->ct = ct+1; // pointer to type -> type
    }

    // + -
    else if(optype == UPLUS_O || optype == UMIN_O)
    {
      // arithmetic
      assert(isarith(ct));

      // perform integral promotion
      newe->args[0] = intprom(newe->args[0]);

      // inherit promoted
      newe->ct = newe->args[0]->ct;
    }

    // ~
    else if(optype == BNOT_O)
    {
      // integral
      assert(isintegral(ct));

      // intprom
      newe->args[0] = intprom(newe->args[0]);

      // inherit promoted
      newe->ct = newe->args[0]->ct;
    }

    // !
    else if(optype == LNOT_O)
    {
      // scalar
      assert(isscalar(ct));

      // result is int
      newe->ct = makedt(INT_T);
    }

    return newe;
  }

  // sizeof
  else if(lisatom(start, SIZEOF))
  {
    // type checks are performed already in sizeoftype

    // puts("hi");
    testerr(start->right, "parseunaryexpr: sizeof has no argument");
    // puts("hi");
    if(lisatom(start->right, PARENOP) && isdeclspec(*start->right->right->cont.tok)) // type-name
    {
      link *cl = findmatch(start->right, RIGHT, PARENOP, PARENCL);
      
      start->right->right->left = NULL;
      cl->left->right = NULL;
      testerr(!cl->right, "parseunaryexpr: extra tokens after (typename)");

      ctype ct = parsetypename(start->right->right);

      testerr(ct, "parseunaryexpr, sizeof type: ct is NULL");
      // evaluate size now
      int size = sizeoftype(ct);
      expr *newe = makeintexpr(size);

      // we need to put it inside a dummy expr because the ctype of newe has to be size_t (int)
      // expr *e = makeexpr(TYPENAME, -1, 0);
      // e->ct = ct;

      // expr *newe = makeexpr(UNAR_E, SIZEOF_O, 1, e);
      // newe->ct = makedt(INT_T);

      return newe;
    }

    else // unary expr
    {
      start->right->left = NULL;

      expr *e = parseunaryexpr(start->right);
      if(!e) return NULL;
      // e->ct = NULL;

      
      testerr(e->ct, "parseunaryexpr, sizeof unary: ct is NULL");
      int size = sizeoftype(e->ct);
      expr *newe = makeintexpr(size);

      // expr *newe = makeexpr(UNAR_E, SIZEOF_O, 1, e);
      // newe->ct = makedt(INT_T);

      return newe;
    }
  }

  // ++, --
  else if(optype == PREINC_O || optype == PREDEC_O)
  {
    start->right->left = NULL;
    expr *e = parseunaryexpr(start->right);
    if(!e) return NULL;

    expr *newe = makeexpr(UNAR_E, optype, 1, e);
    e = newe->args[0];

    // modifiable lvalue
    assert(e->lval);
    assert(ismodifiable(e->ct));

    // scalar
    assert(isscalar(e->ct));
    
    newe->ct = e->ct; // inherit type

    // note that integral promotion does not occur. consider
    /*
  char x = 5;
  putd(sizeof(x+1)); // sizeof(int)
  putd(sizeof(x++)); // 1
    */ 

    return newe;
  }

  else // postfix
  {
    return parsepostexpr(start);
  }
}

ctype parsetypename(link *start)
{
  // here();
  // assert(start);

  testerr(start, "parsetypename: empty start");
  leftend(start);

  // convert to token list
  token *abstype = ll2tokl(start);
  // parse type
  // decl *ct = parsedecl(abstype, 1);
  int i = 0; // set for getdeclspecs()
  decl *dcl = getdeclspecs(abstype, &i);

  if(!dcl) return NULL;
  assert(dcl->storespec == NOSPEC); // no storespecs allowed in casts. in fact we only cary about the ct part of the returned decl

  // extract ct (containing only a TM_DAT right now)
  ctype ct = dcl->ct;
  if(!ct) return NULL;
  gettypemods(abstype, i, -1, 1, NULL, &ct); // write typemods into ct, abstract

  if(!ct) return NULL;

  return ct;
}

expr *parsepostexpr(link *start)
{
  // here();
  // assert(start);
  testerr(start, "parsepostexpr: empty start");
  rightend(start);

  if(lisatom(start, INC)) // a++
  {
    start->left->right = NULL;
    expr *e = parsepostexpr(start->left);
    if(!e) return NULL;

    expr *newe = makeexpr(POST_E, POSTINC_O, 1, e);
    e = newe->args[0];

    assert(e->lval);
    assert(ismodifiable(e->ct));
    assert(isscalar(e->ct));

    newe->ct = e->ct;
    
    return newe;
  }
  if(lisatom(start, DEC)) // a--
  {
    start->left->right = NULL;
    expr *e = parsepostexpr(start->left);
    if(!e) return NULL;

    expr *newe = makeexpr(POST_E, POSTDEC_O, 1, e);
    e = newe->args[0];

    assert(e->lval);
    assert(ismodifiable(e->ct));
    assert(isscalar(e->ct));

    newe->ct = e->ct;
    
    return newe;
  }

  if(lisatom(start->left, DOT)) // a.b
  {
    throw("structs not supported");
    start->left->left->right = NULL;
    expr *e1 = parsepostexpr(start->left->left);
    if(!e1) return NULL;
    start->left = NULL;
    expr *e2 = parseprimexpr(start);
    if(!e2) return NULL;

    expr *newe = makeexpr(POST_E, STRUCT_O, 2, e1, e2);
    return newe;
  }
  if(lisatom(start->left, ARROW)) // a->b
  {
    throw("structs not supported");
    start->left->left->right = NULL;
    expr *e1 = parsepostexpr(start->left->left);
    if(!e1) return NULL;
    start->left = NULL;
    expr *e2 = parseprimexpr(start);
    if(!e2) return NULL;

    expr *newe = makeexpr(POST_E, PSTRUCT_O, 2, e1, e2);
    return newe;
  }

  if(lisatom(start, PARENCL)) // f(a,b,c)
  {
    link *op = findmatch(start, LEFT, PARENCL, PARENOP);

    if(op->left != NULL) // if NULL, it's a (primary-expression)
    {
      // putd(6);
      // here();
      sever(op);
      // printf("%p\n", start->left);
      if(start->left) start->left->right = NULL; // could possibly be set NULL by the above sever()

      expr *e1 = parsepostexpr(op->left);
      if(!e1) return NULL;
      expr *e2 = parsearglist(start->left);
      if(!e2) return NULL;
      // printf("%p\n", start->left);

      expr *newe = makeexpr(POST_E, FUN_O, 2, e1, e2);
      e1 = newe->args[0];
      e2 = newe->args[1];

      ctype ct1 = newe->args[0]->ct;
      ctype ct2 = newe->args[1]->ct;
      // first arg:
      assert(isptr(ct1)); // pointer
      assert(tmis(ct1+1, TM_FUNC)); // to function
      assert(ctisdt(ct1+2, VOID_T) // returning void
          || (isobject(ct1+2) && !tmis(ct1+2, TM_ARR))); // or object type other than array type

      // check function prototype (params) against args
      int np = ct1[1].func.np;
      if(np == -1) ; // unspecified params, let through
      else
      {
        assert(e2->numargs == np); // same number of args
        for(int i = 0; i < np; i++)
        {
          decl *p = ct1[1].func.params + i;

          // types must agree as if by simple assignment
          // assert(iscompat(p->ct, e2->args[i]->ct, QM_SUPERSET));
  // fputs("mi\n", stderr);
  // putctype(p->ct);
  // nline();
  // putctype(e2->args[i]->ct);
  // nline();
          checkasgncompat(EQ_O, p->ct, e2->args[i]->ct);
          e2->args[i] = makecast(p->ct, e2->args[i]);
        }
      }

      newe->ct = ct1 + 2; // * fun ret -> ret
      
      return newe;
    }
  }

  if(lisatom(start, BRACKCL)) // a[i]
  {
    link *op = findmatch(start, LEFT, BRACKCL, BRACKOP);

    // putd(7);
    // here();
    sever(op);
    start->left->right = NULL;

    expr *e1 = parsepostexpr(op->left);
    if(!e1) return NULL;
    expr *e2 = parseexpr(start->left);
    if(!e2) return NULL;

    // expr *newe = makeexpr(POST_E, ARR_O, 2, e1, e2);
    // e1 = newe->args[0];
    // e2 = newe->args[1];

    // ctype ct1 = e1->ct;
    // ctype ct2 = e2->ct;

    
    // instead of ARR_O, convert to the equivalent
    // a[5] -> *(a+5)
    // puts("calling makeexpr");
    expr *adde = makeexpr(ADD_E, ADD_O, 2, e1, e2);
    e1 = adde->args[0];
    e2 = adde->args[1];
    ctype ct1 = e1->ct;
    ctype ct2 = e2->ct;

    expr *newe = makeexpr(UNAR_E, POINT_O, 1, adde);

    // still do the type checks for array indexing
    // one pointer to object, other integral
    if(isptr(ct1) && isintegral(ct2))
    {
      // must be object type
      assert(isobject(ct1+1));
      
      adde->ct = ct1;

      newe->ct = ct1+1;
      newe->lval = 1;
    }

    else if(isintegral(ct1) && isptr(ct2))
    {
      assert(isobject(ct2+1));

      adde->ct = ct2;
      
      newe->ct = ct2+1;
      newe->lval = 1;
    }

    else
    {
      putctype(ct1);
      nline();
      putctype(ct2);
      nline();
      throw("parsepostexpr, ARR_O: bad types");
    }
    
    return newe;
  }

  else // primary expr
  {
    return parseprimexpr(start);
  }
}

expr *parsearglist(link *start)
{
  // here();
    // puts("hi");
    // printf("%p\n", start);
  if(!start) // when severing the parens, this gets set to NULL if the parens are side by side (0 args)
  {
    // puts("bye");
    expr *newe = makeexpr(ARGLIST, -1, 0);
    return newe;
  }

  leftend(start);

  // otherwise, count args
  int numargs = 1; // 0 commas -> 1 arg
  link *temp = start;
  static int cl[] = {COMMA};
  while((temp = nexttoplevel(temp, RIGHT, 1, cl)) != NULL)
  {
    // puttok(*temp->cont.tok);
    numargs++;
    temp = temp->right;
  }

  // make new expr to hold the args
  expr *newe = makeexpr(ARGLIST, -1, 0);
  newe->args = malloc(sizeof(expr) * numargs);
  newe->numargs = numargs;

  // parse args
  leftend(start);
  link *comma;
  for(int i = 0; i < numargs; i++)
  {
    comma = nexttoplevel(start, RIGHT, 1, cl);
    // assert(comma != start); // no empty args
    testerr(comma != start, "parsearglist: empty arg"); // no empty args

    if(comma) // if comma == NULL, last arg only severs on left
    {
      // putd(8);
      // here();
      sever(comma);
    }
    start->left = NULL;

    expr *e = parseasgnexpr(start);
    if(!e) return NULL;

    // decay if necessary
    decay(-1, -1, e);

    newe->args[i] = e;

    if(comma) // if not end, move on
    {
      start = comma->right;
    }
  }

  return newe;
}

void showlithier(char *src, char *esc, char *quot)
{
  int i;
  printf("%s", src);
  for(i = 0; i < strlen(src); i++)
  {
    if(esc[i]) putchar('e');
    else putchar('.');
  }
  puts("");
  for(i = 0; i < strlen(src); i++)
  {
    if(quot[i] == 0)
    {
      putchar('.');
    }
    else
      putchar(quot[i]);
  }
  
}

expr *parseprimexpr(link *start)
{
  // here();
  // assert(start);
  testerr(start, "parseprimexpr: empty start");
  leftend(start);
  

  if(lisatom(start, PARENOP))
  {
    link *cl = findmatch(start, RIGHT, PARENOP, PARENCL);
    // assert(cl->right == NULL); // should be at far right side
    testerr(cl->right == NULL, "parseprimexpr: extra tokens after closing parenthesis"); // should be at far right side
    
    start->right->left = NULL;
    cl->left->right = NULL;
    return parseexpr(start->right);
  }

  // otherwise only one link
  // assert(!start->left);
  testerr(!start->left, "parseprimexpr: extra tokens on left");
  // assert(!start->right);
  testerr(!start->right, "parseprimexpr: extra tokens on right");

  expr *newe = makeexpr(PRIM_E, -1, 0);

  // should not be expr, which is no longer used
  assert(start->type == TOK_L);

  token *t = start->cont.tok;

  // primary expression can't be atom
  testerr(t->gen.type != ATOM, "parseprimexpr: given atom");
  testerr(t->gen.type != KEYWORD, "parseprimexpr: given keyword");

  // idents
  if(t->gen.type == IDENT)
  {
    newe->optype = IDENT_O;

    // search the scope stack from top to bottom for the identifier
    decl *d = searchscope(t->ident.cont);

    // ident must exist in scope
    // assert(d);
    if(!d)
    {
      fprintf(stderr, "%s\n", t->ident.cont);
      throw("could not find identifier in scope");
    }
    // if(streq(d->ident, "factorial"))
    // {
    //   puts("hi");
    //   putparams(d->ct);
    // }

    newe->ct = d->ct;
    newe->dcl = d;

    // double check this dcl is not type void
    assert(!ctisdt(newe->ct, VOID_T));

    if(newe->ct->gen.type != TM_FUNC)
    {
      newe->lval = 1;
    }
  }

  // constants & string literals
  if(t->gen.type == INTEGER)
  {
    newe->optype = INT_O;

    // figure out dt
    if(t->integer.islong)
    {
      if(t->integer.isunsigned)
        newe->ct = makedt(ULINT_T);
      else
        newe->ct = makedt(LINT_T);
    }

    else if(t->integer.isunsigned)
      newe->ct = makedt(UINT_T);
    else
      newe->ct = makedt(INT_T);

    newe->dat = t->integer.cont;
  }

  if(t->gen.type == CHAR)
  {
    newe->optype = CHAR_O;
    newe->ct = makedt(CHAR_T);

    newe->dat = t->character.cont;
  }

  if(t->gen.type == STRLIT)
  {
    newe->optype = STRING_O;
    
    // array of char
    ctype ct = calloc(2, sizeof(typemod));
    // ct->gen.type = TM_ARR;
    // ct->arr.len = t->strlit.len; // length of array, not of string
    // ct[1].gen.type = TM_DAT;
    // ct[1].dat.dt = CHAR_T;
    ct->gen.type = TM_PTR;
    ct[1].gen.type = TM_DAT;
    ct[1].dat.dt = CHAR_T;

    // no modifying the string literal
    // ct[1].dat.isconst = 1;
    newe->ct = ct;
    newe->lval = 1;

    // put string in literals (global mutable variable)

    // label, db
    char *glob = newgloblab();
    vspmac(literals, "%s db ", glob);

    // write string in the form of numbers
    for(int i = 0; i < t->strlit.len; i++)
    {
      vspmac(literals, "%u, ", t->strlit.cont[i]);
    }
    // newline
    appmac(literals, "\n");
    // puts(glob);

    newe->label = glob;
    // putctype(newe->ct);
    // nline();
  }

  if(t->gen.type == FLOATING)
  {
    newe->optype = FLOAT_O;

    // figure out dt
    if(t->floating.islong)
      newe->ct = makedt(LDUB_T);
    else if(t->floating.isshort)
      newe->ct = makedt(FLOAT_T);
    else
      newe->ct = makedt(DUB_T);

    // not sure if this well-defined C, but it works in my tests
    memcpy(&newe->dat, &t->floating.cont, sizeof(float));
    // it would be better to create some interface functions and then store the real thing in an array of 4 bytes, or just a 32 bit field but i don't want to do that
  }
  
  // newe->tok = start->cont.tok;
  // printf("%p\n", newe->tok);
  // puttok(*newe->tok);
  return newe;
}

//{{{1 constant expressions

// // special case, for our limited implementation: all integral constants must be a single primary expression
// dword evalsimpleconstintexpr(expr *e)
// {
//   assert(isintegral(e->ct)); // i.e. INT_O or CHAR_O

//   dword dat = e->dat;

//   // sign extension has already been performed automatically if necessary

//   // get size of the integral type it is (short, long, char, etc.)
//   int size = sizeoftype(e->ct);
  
//   return dat;
// }

// limited support for constant integral expressions (not standard compliant)
// all constants are 4 byte ints, and sign does not matter
dword evalconstintexpr(expr *e)
{
  // alias
  dword (*ecie)(expr *) = evalconstintexpr;

  assert(isintegral(e->ct));
  int ot = e->optype;

  expr **a = e->args;

  if(ot == INT_O || ot == CHAR_O)
    return e->dat;
  else if(ot == CAST_O);
    // do nothing

  else if(ot == ADD_O)
    return ecie(a[0]) + ecie(a[1]);
  else if(ot == SUB_O)
    return ecie(a[0]) - ecie(a[1]);

  else if(ot == UPLUS_O)
    return ecie(a[0]);
  else if(ot == UMIN_O)
    return -ecie(a[0]);
  else if(ot == BNOT_O)
    return ~ecie(a[0]);
  else if(ot == LNOT_O)
    return !ecie(a[0]);

  else if(ot == MULT_O)
    return ecie(a[0]) * ecie(a[1]);
  else if(ot == DIV_O)
    return ecie(a[0]) / ecie(a[1]);
  else if(ot == MOD_O)
    return ((sdword) ecie(a[0])) % (sdword) ecie(a[1]);

  else if(ot == LT_O)
    return ((sdword) ecie(a[0])) < (sdword) ecie(a[1]);
  else if(ot == GT_O)
    return ((sdword) ecie(a[0])) > (sdword) ecie(a[1]);
  else if(ot == LEQ_O)
    return ((sdword) ecie(a[0])) <= (sdword) ecie(a[1]);
  else if(ot == GEQ_O)
    return ((sdword) ecie(a[0])) >= (sdword) ecie(a[1]);
  else if(ot == EQEQ_O)
    return ecie(a[0]) == ecie(a[1]);
  else if(ot == NEQ_O)
    return ecie(a[0]) != ecie(a[1]);

  else if(ot == SHL_O)
    return ecie(a[0]) << ecie(a[1]);
  else if(ot == SHR_O)
    return ecie(a[0]) >> ecie(a[1]);

  else if(ot == BAND_O)
    return ecie(a[0]) & ecie(a[1]);
  else if(ot == XOR_O)
    return ecie(a[0]) ^ ecie(a[1]);
  else if(ot == BOR_O)
    return ecie(a[0]) | ecie(a[1]);

  else if(ot == LOR_O)
    return ecie(a[0]) || ecie(a[1]);
  else if(ot == LAND_O)
    return ecie(a[0]) && ecie(a[1]);

  else if(ot == TERN_O)
  {
    if(ecie(a[0]))
      return ecie(a[1]);
    else
      return ecie(a[2]);
  }
  else
  {
    puts(hropt[ot]);
    throw("unsupported operator");
  }

  return 0;
}

// evaluate a constant expression at compile time
// returns NULL if invalid operators, operands, etc.
// expr *evalconstexpr(expr *e, int integral)
// {
//   // check against blacklisted ops
//   int t = e->type;
//   int o = e->optype;
//   if
//     ( // no assignment, increment/decrement, function calls, commas
//      t == ASGN_E
//      || o == POSTINC_O
//      || o == POSTDEC_O
//      || o == PREINC_O
//      || o == PREDEC_O
//      || o == FUN_O
//      || o == COMMA_O
//     )
//     {
//       return NULL;
//     }

//   // try integral ops first
//   if(o == INT_O)
//   {
    
//   }
// }

//{{{1 string operations

    
// append src to dest, allocating more space as necessary
// assumes dest can be passed to realloc()
char *strapp(char *dest, int *max, char *src)
{
  int len = strlen(dest) + strlen(src) + 1;
  if(len > *max)
  {
    *max = (*max * 2 > len) ? (*max * 2) : len;
    dest = realloc(dest, *max);
  }
  strcat(dest, src);
  return dest;
}

// multiappend
char *multiapp(char *dest, int *max, int n, ...)
{
  va_list ap;
  char *s;

  va_start(ap, n);
  for(int i = 0; i < n; i++)
  {
    s = va_arg(ap, char *);
    dest = strapp(dest, max, s);
  }
  return dest;
}

char *strnew(int n, ...)
{
  va_list ap;

  int max = 1;
  char *dest = malloc(max);
  *dest = 0;
  char *s;

  va_start(ap, n);
  for(int i = 0; i < n; i++)
  {
    s = va_arg(ap, char *);
    dest = strapp(dest, &max, s);
  }

  return dest;
}


// convert a size (in bytes) to the corresponding x86 pseudo-instruction (db and friends)
// NOT for array sizes. e.x. for int arr[5], sizeof(int) should be passed here, not sizeof(int)*5
char *initnasm(int size)
{
  switch(size)
  {
    case 1:
      return "db";
    case 2:
      return "dw";
    case 4:
      return "dd";
    // no more than 4 bytes used in this implementation
    // case 8:
    //   return "dq";
    // case 10:
    //   return "dt";
    // case 16:
    //   return "do";
    default:
      throw("invalid data size");
  }
}

char *sizenasm(int size)
{
  switch(size)
  {
    case 1:
      return "byte";
    case 2:
      return "word";
    case 4:
      return "dword";
    // no more than 4 bytes used in this implementation
    // case 8:
    //   return "dq";
    // case 10:
    //   return "dt";
    // case 16:
    //   return "do";
    default:
      throw("invalid data size");
  }
}

// same as above, but for bss
char *resnasm(int size)
{
  switch(size)
  {
    case 1:
      return "resb";
    case 2:
      return "resw";
    case 4:
      return "resd";
    // no more than 4 bytes used in this implementation
    // case 8:
    //   return "resq";
    // case 10:
    //   return "rest";
    // case 16:
    //   return "reso";
    default:
      throw("invalid data size");
  }
}

// 4 -> "4"
// -4 -> "-4"
char *num2str(int num)
{
  char *s = malloc(12);
  sprintf(s, "%d", num);
  return s;
}

char *regstr(enum gpreg gpr, int size)
{
  assert(size == 1 || size == 2 || size == 4);
  return gpr_ref[gpr][size];
}

// 4 -> "+4"
// -4 -> "-4"
// need the sign in front for the offset
// no bothering with converting to binary because it doesn't matter
char *getoffstr(int off)
{
  // largest displacement is 32 bits -> 10 decimal digits. add on sign and null term, 12 digits
  char *s = malloc(12);

  // sign
  if(off >= 0)
    s[0] = '+';
  else
  {
    s[0] = '-';
    off = -off; // take absolute value
  }

  // write number in after sign
  sprintf(s+1, "%d", off);

  return s;
}

// getbits(256+1, 1) -> "00000001b"
// getbits(256+1, 4) -> "[lots of 0s]100000001b"
// size is in bytes
char *getbits(u_int32_t dat, int size)
{
  int bitsize = size * 8;

  char *s = malloc(bitsize + 2); // number of bits + b + null terminator
  
  // write bits one at a time backwards into s
  for(int i = 1; i <= bitsize; i++)
  {
    s[bitsize - i] = dat % 2 + '0';
    dat >>= 1;
  }

  s[bitsize] = 'b';
  s[bitsize+1] = 0; // null terminate

  return s;
}


// "sub esp, 4"
char *stackalloc(int size)
{
  if(size == 0)
    return "";
  // int size = sizeoftype(d->ct);
  char *sizestr = num2str(size);
  
  return strnew(3, "sub esp, ", sizestr, "\n");
}

char *stackdealloc(int size)
{
  if(size == 0)
    return "";
  char *sizestr = num2str(size);
  
  return strnew(3, "add esp, ", sizestr, "\n");
}

// "mov dword [ebp-4], 3"
char *imm2frame(decl *d, int imm)
{
  char *offstr = getoffstr(d->locat.locloc);
  char *sizestr = sizenasm(sizeoftype(d->ct));
  char *immstr = num2str(imm);
  
  return strnew(7, "mov ", sizestr, " [ebp", offstr, "], ", immstr, "\n");
}

// "mov dword [ebp-4], eax"
char *reg2frame(decl *d, char *reg)
{
  char *offstr = getoffstr(d->locat.locloc);
  char *sizestr = sizenasm(sizeoftype(d->ct));

  return strnew(7, "mov ", sizestr, " [ebp", offstr, "], ", reg, "\n");
}

// "mov eax, dword [ebp-4]
char *stack2frame(char *reg, decl *d)
{
  char *offstr = getoffstr(d->locat.locloc);
  char *sizestr = sizenasm(sizeoftype(d->ct));

  return strnew(8, "mov ", reg, ", ", sizestr, " [ebp", offstr, "]", "\n");
}

// mov dword [esp], eax
// move byte [esp], bl
char *reg2stack(enum gpreg reg, int size)
{
  char *rs = regstr(reg, size);
  char *ss = sizenasm(size);

  char *s;
  asprintf(&s, "mov %s [esp], %s\n", ss, rs);

  return s;
}

char *stack2reg(enum gpreg reg, int size)
{
  char *rs = regstr(reg, size);
  char *ss = sizenasm(size);

  char *s;
  asprintf(&s, "mov %s, %s [esp]\n", rs, ss);

  return s;
}

//{{{1 toplevel

decl *searchscope(char *ident)
{
  decl **dcls = (decl **) scope->cont;
  decl *d;
  
  // search stack from newest (innermost) to oldest (outermost)
  for(int i = scope->n - 1; i >= 0; i--)
  {
    if(!dcls[i]) // NULL separator
      continue;

    // found same ident
    if(streq(dcls[i]->ident, ident))
    {
      // putd(i);
      return dcls[i];
    }
  }

  // couldn't find
  return NULL;
}


// prefixes which act as namespaces to avoid conflicting with each other and builtin features of nasm

// identifiers (variables, functions)
char ident_pre[] = "ident_";

// goto targets
char goto_pre[] = "goto_";

// create a new stack frame at start of function
char create_sframe[] = "push ebp\nmov ebp,esp\n";
// destray a stack frame at end of function
// char destroy_sframe[] = "dbg: mov esp,ebp\npop ebp\n";
char destroy_sframe[] = "mov esp,ebp\npop ebp\n";


// decl *factd = NULL;
// int thththth = 1;

// read top-level decls, process function definitions
// i.e. convert tokens to assembly
void proctoplevel(token *toks)
{
  // allocate list of decls
  // alloc(decl *, alldecls, dsize, dn);

  // initialize literals
  literals = malloc(literals_len);
  *literals = 0;

  // create a stack of decls to be in scope
  scope = makestack(sizeof(decl *));

  contstack = makestack(sizeof(char *));
  breakstack = makestack(sizeof(char *));
  // putp(contstack);
  // putp(breakstack);

  // NULL is a scope separator. it tells us where blocks start, which allows inner identifiers to cover 'hide' outer ones
  // this initial NULL is there for convenience
  pushnull();
  
  // allocate segment buffers (code, data, bss)
  int codeseg_len, dataseg_len, bssseg_len;
  codeseg_len = dataseg_len = bssseg_len = 100;
  char *codeseg = malloc(codeseg_len);
  char *dataseg = malloc(dataseg_len);
  char *bssseg = malloc(bssseg_len);
  *codeseg = *dataseg = *bssseg = 0; // empty string


  decl *d;
  int tokind = 0;
  while((d = parsedecl(toks, &tokind, NULL)) != NULL) // parse until NOTOK
  {
    // puts(d->ident);
    // putctype(d->ct);
    // nline();
    // putparams(d->ct);
    // puts("hi");
    // if(d->ct->gen.type == TM_FUNC)
    // {
    //   puts("ok");
    //   puts(d->ident);
    //   putparams(d->ct);
    // }
    
    // putdecl(d);
    // putd(tokind);

    // if(streq(d->ident, "putstring"))
    // {
    //   // thththth = 0;
    //   // factd = d;
    //   puts("hi");
    //   putparams(d->ct);
    // }

    assert(d->ident); // probably not necessary but good to check that it exists

    // set location and identifier
    d->locat.global = 1;
    d->locat.globloc = strnew(2, ident_pre, d->ident);

    // we do not allow top-level storage class specs, because this implementation does not support multiple translation units. linkage is irrelevant
    assert(d->storespec == NOSPEC);

    // this implementation does not support static or extern at all, for the moment.

    if(d->storespec == K_TYPEDEF)
    {
      // TODO typedef
      throw("typedef isn't supported yet");
    }
    
    assert(!d->fundef || !d->init); // can't have both


    // run through and merge decl with previous duplicates, or append new if unique
    // do this even if fundef appeared above. fundefs are a declaration and a definition at the same time.

    // find initialized declarations, put in data
    // find tentative declarations, put in bss

    // search previous declarations for the same identifier
    int merged = 0;
    // decl **scopearr = (decl **) scope->cont; // extract list from scope stack
    decl *scopedcl;
    for(int i = 0; i < listlen(scope); i++)
    {
    // here(5, putparams(factd->ct));
      listget(scope, i, &scopedcl);
      if(!scopedcl) // skip separators
        continue;

    // here(5, putparams(factd->ct));
      if(streq(scopedcl->ident, d->ident)) // same ident
      {
        // check same type
        assert(iscompat(scopedcl->ct, d->ct, QM_STRICT));

        // check that the inits/fundefs don't conflict, then merge
        if(d->fundef) // function def
        {
          assert(!scopedcl->fundef); // must be unique

          scopedcl->fundef = d->fundef;
        }
        if(d->init) // initialized
        {
          assert(!scopedcl->init); // musn't be prior initialization in this case

          scopedcl->init = d->init;
        }

    // here(5, putparams(factd->ct));
        // merge types
        ctype newct = makecompos(scopedcl->ct, d->ct, QM_STRICT); // write into alldecls[i] the composite type
    // here(5, putparams(factd->ct));
        scopedcl->ct = newct;
        
    // here(5, putparams(factd->ct));
        // indicate the decl already exists
        merged = 1;
        break;
      }

    }
    // here(5);
    // putparams(factd->ct);
    // here(5, putparams(factd->ct));

    if(!merged) // it's the first occurrence, write into alldecls
    {
      // resize(alldecls, dsize, scope->n);
      // alldecls[scope->n++] = d;
      push(scope, &d);

      // puts(d->ident);
      // putparams(d->ct);
      // fputs("hi",stderr);
      // fprintf(stderr, "%s\n", d->ident);
      // for(int i = 0; i < d->ct->func.np; i++)
      // {
      //   putctype(d->ct->func.params[i].ct);
      //   printf(", ");
      // }
      // putctype(d->ct);
      // nline();
      // nline();
    }

    // here(5);
    // putparams(factd->ct);
    
    // fundefs are parsed after their declaration is considered above, so that recursion is possible (the function is in its own scope)
    if(d->fundef) // if fundef, parse now
    {
      // double check it's a function
      assert(d->ct->gen.type == TM_FUNC);

      // push separator
      pushnull();

      // look at prototype, put declarations into scope
      decl *curdcl;
      ctype ct = d->ct;

      // offset is how far above the base pointer we are
      // starts with the sizes of ebp and eip which are already on the stack
      int offset = PTR_SIZE + PTR_SIZE;

      for(int j = 0; j < ct->func.np; j++)
      {
        curdcl = ct->func.params + j;
        pushdecl(curdcl);

        // set location to offset, increase offset by size
        ct->func.params[j].locat = (struct location) {.global = 0, .locloc = offset};
        offset += sizeoftype(ct->func.params[j].ct);
      }

      // start function, create stack frame
      // codeseg = multiapp(codeseg, &cs_len, 4, "\n", d->locat.globloc, ":\n", create_sframe);
      mapmac(codeseg, "\n", d->locat.globloc, ":\n", create_sframe);
      
      // indicate that a function is just starting (must be block statement, no pushing another separator
      // startfundef = 1;
      // indicate desired return type
      funret = ct+1;

      // convert function body to assembly
      char *s = parsestat(d->fundef, 1, 1);
    // here(5);
    // putparams(factd->ct);

      funret = NULL; // not necessary, but safer

      // append assembly to code segment
      // codeseg = strapp(codeseg, &cs_len, s);
      appmac(codeseg, s);
      
      // write assembly
      // end function (in case falls off the end)
      // codeseg = multiapp(codeseg, &cs_len, 2, destroy_sframe, "ret\n");
      // vspmac(codeseg, "%s:\n", newgloblab());
      mapmac(codeseg, destroy_sframe, "ret\n\n");
    }
    
    // here(5);
    // putparams(factd->ct);
  }

  // decl **scopearr = (decl **) scope->cont; // extract list from scope stack
  // run through a second time, this time allocating storage, initializing, etc.
  for(int i = 0; i < listlen(scope); i++)
  {
    // d = scopearr[i];
    listget(scope, i, &d);

    if(!d) // skip separators
      continue;

    if(d->ct->gen.type == TM_FUNC) // function decl, nothing to initialize
    {
      continue;
    }

    if(d->init) // evaluate initializer, put in data
    {
      // assert(!d->init->islist); // list initializers not supported yet

      // assert(d->ct->gen.type != TM_ARR); // lists not supported, therefore arrays with init not supported
      // this also saves us the problem of more complicated memory reservations

      char *value = NULL;
      int size = sizeoftype(d->ct);
      expr *e = d->init->e;

      if(d->ct->gen.type  == TM_ARR)
      {
        assert(d->init->islist);

        // if(!ctisdt(d->ct+1, CHAR_T))
        // {
          throw("toplevel array init not supported yet");
        // }
      }
      
      else if(isintegral(d->ct))
      {
        assert(!d->init->islist);

        sdword num = evalconstintexpr(e);
        value = getbits(num, size);
      }

      else
      {
        throw("unsupported toplevel initializer");
      }

      // char *value = getbits(e->dat, size);

      char *def = initnasm(size); // db, dw, etc.
      // dataseg = multiapp(dataseg, &ds_len, 6, d->locat.globloc, " ", def, " ", value, "\n"); // TODO replace "1" with actual init value
      mapmac(dataseg, d->locat.globloc, " ", def, " ", value, "\n");
    }
    else // no init (all tentative -> 0), put in bss
    {
      // int count = 0; // arrays have to reserve multiple. everything else only reserves one
      // int size = 0;

      // if(d->ct->gen.type == TM_ARR)
      // {
      //   if(d->ct->arr.len == -1)
      //   {
      //     d->ct->arr.len = 1; // default to 1
      //   }
      //   count = d->ct->arr.len;
      //   size = sizeoftype(d->ct+1); // remove TM_ARR, get size
      // }

      // else // not array, get type directly
      // {
      //   count = 1;
      //   size = sizeoftype(d->ct);
      // }

      // char countstr[100]; // no way count is over 100 digits
      // sprintf(countstr, "%d", count); // write count into countstr

      // char *res = resnasm(size);
      // // bssseg = multiapp(bssseg, &bs_len, 7, ident_pre, d->ident, " ", res, " ", countstr, "\n");
      // mapmac(bssseg, ident_pre, d->ident, " ", res, " ", countstr, "\n");

      // simply reserve the number of bytes required
      int size = sizeoftype(d->ct);
      char *sizestr = num2str(size);
      mapmac(bssseg, ident_pre, d->ident, " resb ", sizestr, "\n");
    }
  }

  // append string literal definitions to data segment
  appmac(dataseg, literals);

  // write _start function which calls main
  
  // codeseg = strapp(codeseg, &cs_len,"global _start\n_start:\n  call ident_main\n  call exit\n\nexit:\n  mov eax, 1\n  mov ebx, 0\n  int 80h\n");
  appmac(codeseg,
      "extern ident_putchar\n"
      "extern ident_getchar\n"
      "extern ident_exit\n"
      "global _start\n"
      "_start:\n"
      "call ident_main\n"
      "call exit\n\n"
      "error:\n"
      "mov edx, errlen\n"
      "mov ecx, errmsg\n"
      "mov ebx, 1\n"
      "mov eax, 4\n"
      "int 0x80\n"
      "exit:\n"
      "mov eax, 1\n"
      "mov ebx, 0\n"
      "int 80h\n"
      );

  appmac(dataseg,
      "errmsg db \"error\", 10\n"
      "errlen equ $ - errmsg\n"
      );
  
  
  printf("section .data\n%s\nsection .bss\n%s\nsection .text\n%s\n", dataseg, bssseg, codeseg);
}


//{{{1 scope operations

// #define pushnull() {void *null = NULL; push(scope, &null);}
// push NULL separator onto stack
void pushnull()
{
  void *null = NULL;
  push(scope, &null);
}

// delete everything up to and including next NULL separator on scope stack
// return the sum of the sizes of the popped decls
int remtonull()
{
  decl *d;
  int size = 0;

  // pop until NULL is popped
  do
  {
    pop(scope, &d);
    if(d)
    {
      size += sizeoftype(d->ct);
    }
  } while(d != NULL);

  return size;
}

// push declaration to stack, search until separator for identifier collisions
void pushdecl(decl *d)
{
  decl *curdcl;
  // search backward until separator for declaration for the same identifier
  for(int i = listlen(scope) - 1; i >= 0; i--)
  {
    listget(scope, i, &curdcl);

    // reached separator, stop checking for conflicts
    if(!curdcl)
      break;
    
    // make sure identifiers don't equal
    assert(!streq(curdcl->ident, d->ident));
  }

  // push new decl
  push(scope, &d);
}


//{{{1 statement eval

#define sall(size) {appmac(assem, stackalloc(size)); stacksize += size;}
#define sdall(size) {appmac(assem, stackdealloc(size)); stacksize -= size;}

// safety to ensure that allocated space is properly deallocated
int stacksize = 0;

// create new local (dot prefixed) label
char *newloclab()
{
  static char *pre = ".lab";
  static int num = 0;

  char *s;
  asprintf(&s, "%s%d", pre, num);

  num++;

  return s;
}

char *newgloblab()
{
  static char *pre = "glob";
  static int num = 0;

  char *s;
  asprintf(&s, "%s%d", pre, num);

  num++;

  return s;
}

int findatom(token *toks, int i, int dir, enum atom_type t)
{
  while(!tisatom(toks[i], t))
  {
    if(toks[i].gen.type == NOTOK || i < 0)
    {
      puts(hrat[t]);
      throw("could not find atom");
    }

    i += dir;
  }

  return i;
}

// return index of matching atom (e.x. BRACEOP and BRACECL) in token list
// gives error if goes out of bounds
int tokmatch(token *toks, int i, int dir, enum atom_type beg, enum atom_type end)
{
  int dep = 0;

  // check that we've been given a proper starting index
  assert(tisatom(toks[i], beg));
  
  do
  {
    if(tisatom(toks[i], beg)) dep++;
    if(tisatom(toks[i], end)) dep--;

    assert(dep >= 0);
    assert(toks[i].gen.type != NOTOK);
    assert(i >= 0);

    i += dir;
  } while(dep != 0);

  return i - dir;
}

// parses one statement, moves lo forward accordingly
char *parsestat(struct stat *stat, int nodecl, int startfundef)
{
  int lo = stat->lo;
  int hi = stat->hi;
  token *toks = stat->toks;
  int toklen = hi - lo + 1;

  // all assembly code from the statements to be parsed will be appended to this buffer
  int assem_len = 100;
  char *assem = malloc(assem_len);
  *assem = 0;


  // TODO declarations mixed in with statements

  // if starting fundef, next statement MUST be block statement
  if(startfundef)
    assert(tisatom(toks[lo], BRACEOP));

  // block statements
  if(tisatom(toks[lo], BRACEOP))
  {
    // push separator to scope if not at start of function definition block
    if(startfundef)
    {
      // indicate start of new stack frame (for declarations)
      framesize = 0;
    }
    else
    {
      pushnull();
    }

    // find matching brace
    int end = tokmatch(toks, lo, 1, BRACEOP, BRACECL);

    // we do not confine this implementation to disallowing mixed declarations and statements. it adds no complexity to allow mixing

    // run through statements
    struct stat newstat = {toks, lo+1, end-1};
    char *newassem = parsestats(&newstat);

    if(startfundef)
    {
      // allocate the framesize onto the stack, based on all the declarations encountered within the block statement above
      appmac(assem, stackalloc(framesize));

      // deallocation is done by destroy_sframe at the end
    }
    
    // append newassem to assem
    // assem = strapp(assem, &assem_len, newassem);
    appmac(assem, newassem);

    // roll back scope
    remtonull();

    // deallocate all of them
    // sdall(size);

    // TODO deallocate on real stack as well

    // move on to next statement
    lo = end + 1;
  }

  // labeled statements
  // case label
  else if(tiskeyword(toks[lo], K_CASE))
  {
    throw("switch-case not supported");
    // TODO only in switch. need to pass information downward.
    // find matching colon, isolate constant expression
  }

  // default label
  else if(toklen >= 2 && tiskeyword(toks[lo], K_DEFAULT) && tisatom(toks[lo+1], COLON))
  {
    
    throw("switch-case not supported");
  }

  // regular label
  else if(toklen >= 2 && toks[lo].gen.type == IDENT && tisatom(toks[lo+1], COLON))
  {
    vspmac(assem, "%s%s:\n", goto_pre, toks[lo].ident.cont);

    lo += 2;

    // the expression following the label is required
    assert(lo <= hi);
    struct stat labstat = {toks, lo, hi};
    
    // gcc without options still forbids labelled declarations for some reason, so nodecl is 1
    appmac(assem, parsestat(&labstat, 1, 0));
    lo = labstat.lo;
  }

  // selection statements
  // if
  else if(tiskeyword(toks[lo], K_IF))
  {
    // assert(tisatom(toks[lo+1], PARENOP));
    assert(tisatom(toks[lo+1], PARENOP));

    // find matching paren
    int match = tokmatch(toks, lo+1, 1, PARENOP, PARENCL);

    // parse expression between parens
    expr *e = tokl2expr(toks, lo+2, match-1);
    assert(e);
    int esize = sizeoftype(e->ct);

    // evaluate, put result on stack
    appmac(assem, evalexpr(e));


    // move into eax, test 
    appmac(assem, stack2reg(EAX, esize));
    sdall(esize);
    vspmac(assem, "test %s, %s\n", regstr(EAX, esize), regstr(EAX, esize));

    // jump to lab1 (don't run next statement) if 0
    char *lab1 = newloclab();
    vspmac(assem, "jz %s\n", lab1);

    // evaluate the next statement (the one that runs if the condition is true
    struct stat ifstat = {toks, match + 1, hi};
    appmac(assem, parsestat(&ifstat, 1, 0));
    lo = ifstat.lo; // move over

    // jump past else clause (if else clause exists) if if clause executes
    char *lab2 = newloclab();
    vspmac(assem, "jmp %s\n", lab2);

    // lab1, where we jump if the condition is false
    vspmac(assem, "%s:\n", lab1);

    // check for else clause
    if(tiskeyword(toks[lo], K_ELSE))
    {
      struct stat elsestat = {toks, lo+1, hi};
      // evaluate
      appmac(assem, parsestat(&elsestat, 1, 0));
      lo = elsestat.lo; // move over
    }

    // where to jump to skip else clause (if it exists)
    vspmac(assem, "%s:\n", lab2);

  }

  // switch
  else if(tiskeyword(toks[lo], K_SWITCH))
  {
    assert(tisatom(toks[lo+1], PARENOP));
    throw("switch-case not supported");
  }


  // iteration statements
  // while
  else if(tiskeyword(toks[lo], K_WHILE))
  {
    assert(tisatom(toks[lo+1], PARENOP));

    /*
       .lab1
       [eval condition]
       test eax, eax
       jz .lab2
       [statement]
       jmp .lab1
    */

    char *lab1 = newloclab();
    char *lab2 = newloclab();

    // find matching paren
    int match = tokmatch(toks, lo+1, 1, PARENOP, PARENCL);
    
    // parse expression between parens
    expr *e = tokl2expr(toks, lo+2, match-1);
    assert(e);
    int esize = sizeoftype(e->ct);

    // lab1, to jump back to at bottom of loop
    vspmac(assem, "%s:\n", lab1);

    appmac(assem, evalexpr(e));
    appmac(assem, stack2reg(EAX, esize));
    sdall(esize);
    vspmac(assem, "test %s, %s\n", regstr(EAX, esize), regstr(EAX, esize));

    // jump past loop if result is zero
    vspmac(assem, "jz %s\n", lab2);
    
    // push continue and break targets to stacks
    push(contstack, &lab1);
    push(breakstack, &lab2);

    // evaluate next statement
    struct stat whilestat = {toks, match + 1, hi};
    appmac(assem, parsestat(&whilestat, 1, 0));
    lo = whilestat.lo; // move over

    // pop continue and break targets
    pop(contstack, NULL);
    pop(breakstack, NULL);

    // jump back to top
    vspmac(assem, "jmp %s\n", lab1);
    // where to jump when ending the loop
    vspmac(assem, "%s:\n", lab2);
  }

  // do/while
  else if(tiskeyword(toks[lo], K_DO))
  {
    // do stat while(expr);
    char *lab1 = newloclab();
    char *lab2 = newloclab(); // needed for continue keyword
    char *lab3 = newloclab(); // needed for break keyword

    // jump here when test passes
    vspmac(assem, "%s:\n", lab1);

    // push continue and break statements
    push(contstack, &lab2);
    push(breakstack, &lab3);

    // evaluate next statement
    struct stat dostat = {toks, lo+1, hi};
    appmac(assem, parsestat(&dostat, 1, 0));
    lo = dostat.lo; // move over

    pop(contstack, NULL);
    pop(breakstack, NULL);
    
    // check `while(expr);` format
    assert(tiskeyword(toks[lo], K_WHILE));
    assert(tisatom(toks[lo+1], PARENOP));
    int match = tokmatch(toks, lo+1, 1, PARENOP, PARENCL);
    assert(tisatom(toks[match+1], SEMICOLON));

    // parse expr
    expr *e = tokl2expr(toks, lo+2, match-1);
    assert(e);
    int esize = sizeoftype(e->ct);

    vspmac(assem, "%s:\n", lab2);
    
    // evaluate expr
    appmac(assem, evalexpr(e));
    appmac(assem, stack2reg(EAX, esize));
    sdall(esize);
    
    // test
    vspmac(assem, "test %s, %s\n", regstr(EAX, esize), regstr(EAX, esize));
    // jump back if nonzero
    vspmac(assem, "jnz %s\n", lab1);
    // otherwise keep going (no other jumps, must execute at least once)
    vspmac(assem, "%s:\n", lab3);
  }

  // for
  else if(tiskeyword(toks[lo], K_FOR))
  {
    assert(tisatom(toks[lo+1], PARENOP));
    int pstart = lo+1;

    // find matching paren
    int pend = tokmatch(toks, lo+1, 1, PARENOP, PARENCL);

    // find two separating semicolons
    int sc1 = findatom(toks, lo+2, 1, SEMICOLON);
    int sc2 = findatom(toks, sc1+1, 1, SEMICOLON);

    // parse & evaluate first expression
    // only if not empty
    if(pstart+1 <= sc1-1)
    {
      expr *e1 = tokl2expr(toks, pstart+1, sc1-1);
      int esize = sizeoftype(e1->ct);
      appmac(assem, evalexpr(e1));

      // discard result
      sdall(esize);
    }

    char *lab1 = newloclab(); // start of loop
    char *lab2 = newloclab(); // right before expression 3
    char *lab3 = newloclab(); // end

    vspmac(assem, "%s:\n", lab1);

    // if second expression nonempty, evaluate
    if(sc1+1 <= sc2-1)
    {
      // eval condition
      expr *e2 = tokl2expr(toks, sc1+1, sc2-1);
      int esize = sizeoftype(e2->ct);
      appmac(assem, evalexpr(e2));

      // test condition
      appmac(assem, stack2reg(EAX, esize));
      sdall(esize);
      vspmac(assem, "test %s, %s\n", regstr(EAX, esize), regstr(EAX, esize));

      // jump to end if zero
      vspmac(assem, "jz %s\n", lab3);
    }
    // otherwise condition is always true, don't test anything

    // push continue and break statements
    // putd(breakstack->n);
    push(breakstack, &lab3);
    // putd(contstack->n);
    push(contstack, &lab2);
    
    // eval enclosed statement
    struct stat forstat = {toks, pend+1, hi};
    appmac(assem, parsestat(&forstat, 1, 0));
    lo = forstat.lo; // move over

    // putd(breakstack->n);
    pop(breakstack, NULL);
    // putd(contstack->n);
    pop(contstack, NULL);

    // evaluate 3rd expression
    vspmac(assem, "%s:\n", lab2);

    if(sc2+1 <= pend-1)
    {
      expr *e3 = tokl2expr(toks, sc2+1, pend-1);
      int esize = sizeoftype(e3->ct);
      appmac(assem, evalexpr(e3));

      // discard result
      sdall(esize);
    }

    // jump back to top
    vspmac(assem, "jmp %s\n", lab1);
    // where to jump when ending the loop
    vspmac(assem, "%s:\n", lab3);
  }

  // jump statements
  // goto
  else if(tiskeyword(toks[lo], K_GOTO))
  {
    // goto identifier;
    assert(toks[lo+1].gen.type == IDENT);
    assert(tisatom(toks[lo+2], SEMICOLON));

    // prepend namespace, jump to identifier
    vspmac(assem, "jmp %s%s\n", goto_pre, toks[lo+1].ident.cont);

    // move over
    lo += 3;
  }

  // continue
  else if(tiskeyword(toks[lo], K_CONTINUE))
  {
    // check semicolon
    assert(tisatom(toks[lo+1], SEMICOLON));
    // move over
    lo += 2;
    
    // get top of contstack
    char *lab;
    peek(contstack, &lab);
    
    // jump there
    vspmac(assem, "jmp %s\n", lab);
  }

  // break
  else if(tiskeyword(toks[lo], K_BREAK))
  {
    // check semicolon
    assert(tisatom(toks[lo+1], SEMICOLON));
    // move over
    lo += 2;

    // get top of breakstack
    char *lab;
    peek(breakstack, &lab);

    // jump there
    vspmac(assem, "jmp %s\n", lab);
  }

  // return
  else if(tiskeyword(toks[lo], K_RETURN))
  {
    // find semicolon
    // puts("heuootueu");
    int end = findatom(toks, lo, 1, SEMICOLON);

    assert(funret); // make sure a return type has been provided

    // expression is present
    if(end > lo + 1)
    {
      // make sure not void
      assert(!ctisdt(funret, VOID_T));

      // parse expression
      expr *e = tokl2expr(toks, lo+1, end-1);

      // check that type matches return type, as if by assignment
      assert(iscompat(funret, e->ct, QM_SUPERSET));

      int retsize = sizeoftype(funret);

      // evaluate expression, place result in eax
      appmac(assem, evalexpr(e));
      appmac(assem, stack2reg(EAX, retsize));
      sdall(retsize);
    }

    // no expression is present
    else
    {
      // must be void
      assert(ctisdt(funret, VOID_T));
    }

    // destroy stack frame, return
    // assem = multiapp(assem, &assem_len, 2, destroy_sframe, "ret\n");
    // vspmac(assem, "%s:\n", newgloblab());
    mapmac(assem, destroy_sframe, "ret\n");

    // move on
    // we CANNOT ignore stuff after a return, because of gotos, etc.
    lo = end + 1;
  }

  // penultimate: indoor (local) declaration
  else if(isdeclspec(toks[lo])) // decl spec -> declaration
  {
    // strict C forbids mixing declarations with expressions, but it is not hard to implement anyway, so we allow it.
    // however some cases (e.x. `if(1) int x;`) where it is not directly inside a block statement, this is not good, because block statements handle the deallocation of declarations, therefore they have to be disallowed there
    assert(!nodecl);

    // find terminating semicolon, just to confirm that it exists
    // puts("hey");
    int end = findatom(toks, lo, 1, SEMICOLON);

    // while no semicolon reached, parse declarations
    int sc = 0;
    decl *d;
    do
    {
      d = parsedecl(toks, &lo, &sc);
      assert(d);

      // putdecl(d);

      ctype ct = d->ct;
      int size = sizeoftype(ct);

      // no fundefs indoors
      assert(!d->fundef);

      // push the declaration onto the scope stack
      pushdecl(d);

      // allocate on stack (keep esp at top of stack)
      // appmac(assem, stackalloc(sizeoftype(d->ct)));
      // sall(sizeoftype(d->ct));

      // figure out where it goes on the real stack
      framesize += size;
      d->locat = (struct location) {.global = 0, .locloc = -framesize}; // give local location, offset from base pointer

      // initialize if necessary
      if(d->init)
      {
        // no lists for now
        assert(!d->init->islist);
        assert(d->ct->gen.type != TM_ARR);

        ctype ct1 = d->ct;

        decay(-1, -1, d->init->e);
        ctype ct2 = d->init->e->ct;
        // make sure valid assignment
  // fputs("oi\n", stderr);
        checkasgncompat(EQ_O, ct1, ct2);

        // cast if necessary
        if(!iscompat(ct1, ct2, QM_STRICT))
        {
          d->init->e = makecast(ct1, d->init->e);
        }

        // create the expressions (LHS identifier, RHS value)
        expr *de = makeexpr(PRIM_E, IDENT_O, 0);
        de->dcl = d;
        de->ct = d->ct;

        // put into EQ_O assignment expression
        expr *newe = makeexpr(ASGN_E, EQ_O, 2, de, d->init->e);
        newe->ct = d->ct;

        // evaluate (perform assignment)
        appmac(assem, evalexpr(newe));
        // discard value of assignment expression
        sdall(sizeoftype(d->ct));

        // appmac(assem, imm2frame(d, d->init->e->dat));
      }

    } while(!sc);

    // lo is automatically moved over the semicolon by parsedecl()
  }

  // lastly, if none of those things, expression
  else
  {
    // find semicolon
    int end = findatom(toks, lo, 1, SEMICOLON);

    if(lo == end) // empty expression, move over
    {
      lo++;
    }

    else
    {
      // convert expression to assembly, append
      expr *e = tokl2expr(toks, lo, end-1);

      // puts("eouoeueueueuoeue");
      // putexpr(e);
      // puts("eoueou");

      // toplevel expr decays always, as it's not the operand of something preventing it from decaying
      decay(-1, -1, e);

      // putexpr(e);

      // evaluate, put on stack
      char *s = evalexpr(e);
      appmac(assem, s);

      // get size after decay
      if(!eisdt(e, VOID_T)) // if not void, deallocate returned value
      {
        int size = sizeoftype(e->ct);

        // expression statement value gets thrown out
        appmac(assem, stackdealloc(size));
      }

      // move over semicolon
      lo = end + 1;
    }
  }
  
  // pack back into stat
  stat->lo = lo;
  stat->hi = hi;

  return assem;
}

// take in 1 or more statements strung together, all of them, modify stat accordingly, return assembly code
// also deals with internal declarations, which can appear mixed with statements
char *parsestats(struct stat *stat)
{
  int lo = stat->lo;
  int hi = stat->hi;
  token *toks = stat->toks;
  int toklen = hi - lo + 1;

  // all assembly code from the statements to be parsed will be appended to this buffer
  int assem_len = 100;
  char *assem = malloc(assem_len);
  *assem = 0;

  // putd(lo);
  // putd(hi);
  // putd(toklen);
  // assert(toklen >= 1); // non empty
  // e.x. empty block statement, can start empty


  while(stat->lo <= stat->hi) // while there are tokens left in the given range
  {
    appmac(assem, parsestat(stat, 0, 0));
  }

  return assem;
}


//{{{1 expr eval

// given global or local location (label or offset from ebp), put address of location on stack
char *pushlocat(struct location locat)
{
  int assem_len = 100;
  char *assem = malloc(assem_len);
  *assem = 0;
  
  sall(PTR_SIZE);
  
  // global, label
  if(locat.global)
  {
    vspmac(assem, "mov %s [esp], %s\n", sizenasm(PTR_SIZE), locat.globloc);
  }
  // local
  else
  {
    // lea eax, [ebp-5]
    vspmac(assem, "lea eax, [ebp%s]\n", getoffstr(locat.locloc));
    // mov dword [esp], eax
    vspmac(assem, "mov %s [esp], eax\n", sizenasm(PTR_SIZE));
  }

  return assem;
}

// convert one integral type to another
// integer is assumed to be in gpr a (eax, ax, or al)
// result should also be in gpr a
// modifies: eax and ebx
char *evalintcast(ctype to, ctype from)
{
  int assem_len = 100;
  char *assem = malloc(assem_len);
  *assem = 0;
  
  assert(isintegral(to));
  assert(isintegral(from));
  
  int tosize = dtsize(to->dat.dt);
  int fromsize = dtsize(from->dat.dt);
  int signedto = issigned(to);
  int signedfrom = issigned(from);

  // promoting
  if(tosize > fromsize)
  {
    // sign extend
    if(signedfrom)
    {
      // move from stack to gpr a, deallocate
      vspmac(assem, "mov %s, %s [esp]\n", regstr(EAX, fromsize), sizenasm(fromsize));
      sdall(fromsize);
      
      vspmac(assem, "movsx %s, %s\n", regstr(EAX, tosize), regstr(EAX, fromsize));
      
      // put back on stack
      sall(tosize);
      vspmac(assem, "mov %s [esp], %s\n", sizenasm(tosize), regstr(EAX, tosize));
    }

    // zero extend
    else
    {
      // move from stack to gpr a, deallocate
      vspmac(assem, "mov %s, %s [esp]\n", regstr(EAX, fromsize), sizenasm(fromsize));
      sdall(fromsize);
      
      vspmac(assem, "movzx %s, %s\n", regstr(EAX, tosize), regstr(EAX, fromsize));

      // put back on stack
      sall(tosize);
      vspmac(assem, "mov %s [esp], %s\n", sizenasm(tosize), regstr(EAX, tosize));
    }
  }

  // sidemoting: do nothing
  else if(tosize == fromsize) ;
  
  // demoting
  else // truncate: simply change size of the thing on stack
  {
    // move from stack to gpr a, deallocate
    vspmac(assem, "mov %s, %s [esp]\n", regstr(EAX, fromsize), sizenasm(fromsize));
    sdall(fromsize);

    // put back on stack
    sall(tosize);
    vspmac(assem, "mov %s [esp], %s\n", sizenasm(tosize), regstr(EAX, tosize));
  }

  return assem;
}


char *dbgstatus = "start";
// expr to asm. asm calculates the value of e and puts the result on the stack
char *evalexpr(expr *e)
{
  int assem_len = 100;
  char *assem = malloc(assem_len);
  *assem = 0;

  int ot = e->optype;

  int size = safesizeoftype(e->ct);
  // if(!eisdt(e, VOID_T))
  //   size = sizeoftype(e->ct);
  // else
  //   size = 0; // make sure we don't allocate anything, by just passing size 0

  // allocate size on stack
  // appmac(assem, stackalloc(size));
  // sall(size);

  // remember current stack size
  int curstacksize = stacksize;
  
  // identifier: get value/decay
  if(ot == IDENT_O)
  {
    sall(size); // immediately allocate, no intermediate stack work needed
    
    decl *dcl = e->dcl;

    char *sizestr = sizenasm(size);

    // we know we're moving something onto the stack immediately
    // mapmac(assem, "mov ", sizestr, " [esp], ");

    // type of declaration, NOT type of expr. expr already has been decayed to satisfy type checker
    int tmt = dcl->ct->gen.type;
    
    // global variable
    if(dcl->locat.global)
    {
      // array or function, decay to pointer
      // decay to pointer simply means copy the address over
      if(tmt == TM_ARR
          || tmt == TM_FUNC)
      {
        // imm2mem
        mapmac(assem, "mov ", sizestr, " [esp], ", dcl->locat.globloc, "\n");
      }

      // mapmac(assem, "mov ", sizenasm(size), " [esp], ", dcl->locat.globloc, "\n");
      else // decay to value
      {
        // mem2reg, reg2mem
        char *rs = regstr(EAX, size);
        mapmac(assem, "mov ", rs, ", ", sizestr, " [", dcl->locat.globloc, "]\n");
        mapmac(assem, "mov ", sizestr, " [esp], ", rs, "\n");
        // mapmac(assem, "mov ", sizestr, " [esp], ", "[", dcl->locat.globloc, "]", "\n");
      }
    }

    // local variable, get base pointer offset
    else
    {
      // no local functions
      assert(tmt != TM_FUNC);

      // array decay to pointer
      if(tmt == TM_ARR)
      {
        // lea2reg, reg2mem
        mapmac(assem, "lea eax, ", "[ebp", getoffstr(dcl->locat.locloc), "]\n");
        mapmac(assem, "mov ", sizestr, " [esp], eax\n");
      }

      // decay to value
      else
      {
        char *rs = regstr(EAX, size);
        // mem2reg, reg2mem
        mapmac(assem, "mov ", rs, ", ", sizestr, " [ebp", getoffstr(dcl->locat.locloc), "]\n");
        mapmac(assem, "mov ", sizestr, " [esp], ", rs, "\n");
      }
      
      // mapmac(assem, "mov ", sizenasm(size), " [esp], ", "[ebp", getoffstr(dcl->locat.locloc), "]\n");
    }

    dbgstatus = "IDENT_O";
  }

  // integral constant, 5, 'a'
  else if(ot == INT_O || ot == CHAR_O)
  {
    char *sizestr = sizenasm(size);
    // allocate
    sall(size);
    // fill
    mapmac(assem, "mov ", sizestr, " [esp], ", getbits(e->dat, size), "\n");

    dbgstatus = "INT_O, CHAR_O";
  }

  // string literal, which we treat as char *. put label on stack
  else if(ot == STRING_O)
  {
    // need a label to access string literal
    assert(e->label);
    sall(PTR_SIZE);
    vspmac(assem, "mov %s [esp], %s\n", sizenasm(PTR_SIZE), e->label);
  }

  // unary *
  else if(ot == POINT_O)
  {
    int targsize = sizeoftype(e->ct);

    // put address on stack
    char *s = evalexpr(e->args[0]);
    mapmac(assem, s);

    // put address in eax
    mapmac(assem, "mov eax, ", sizenasm(PTR_SIZE), " [esp]\n");
    
    // check for null pointer, if so, shortcircuit to error
    mapmac(assem, "test eax, eax\nje near error\n");
    
    // deallocate address from stack
    // mapmac(assem, stackdealloc(PTR_SIZE));
    sdall(PTR_SIZE);

    // allocate space for target
    sall(targsize);

    // special case, dereferencing pointer to array of T, returns the same pointer (with type array now, which decays to poiinter to T)
    if(e->args[0]->ct[1].gen.type == TM_ARR)
    {
      mapmac(assem, "mov ", sizenasm(PTR_SIZE), " [esp], eax\n");
    }
    
    else
    {
      char *sizestr = sizenasm(targsize);
      char *rs = regstr(EAX, targsize);
      
      // mov eax, [eax] or similar
      mapmac(assem, "mov ", rs, ", ", sizestr, " [eax]\n");
      
      // move data onto stack
      mapmac(assem, "mov ", sizestr, " [esp], ", rs, "\n");
    }

    dbgstatus = "POINT_O";
  }

  // unary &
  else if(ot == ADDR_O)
  {
    
    // special form, behaves differently based on what its argument is

    // it might be cleaner to have every lvalue be a POINT_O of the address, and then ADDR_O just cancels that out, but this way seems to work fine

    // & * cancels out
    // ex. &*(a+5) -> a+5
    if(e->args[0]->optype == POINT_O)
    {
      char *s = evalexpr(e->args[0]->args[0]);
      mapmac(assem, s);
    }

    // ident/decl: extract location, put on stack
    else if(e->args[0]->optype == IDENT_O)
    {
      assert(e->args[0]->dcl);
      decl *dcl = e->args[0]->dcl;
      
      // put address of dcl on stack
      char *s = pushlocat(dcl->locat);
      appmac(assem, s);
      // sall(size);
      // // global
      // if(dcl->locat.global)
      // {
      //   mapmac(assem, "mov ", sizenasm(PTR_SIZE), " [esp], ", dcl->locat.globloc, "\n");
      // }

      // // local
      // else
      // {
      //   mapmac(assem, "lea eax, ", "[ebp", getoffstr(dcl->locat.locloc), "]\n");
      //   mapmac(assem, "mov ", sizenasm(PTR_SIZE), " [esp], eax\n");
      // }
      
    }

    else
    {
      throw("unanticipated usage of unary &");
    }

    dbgstatus = "ADDR_O";
  }

  // simple assignment
  else if(ot == EQ_O)
  {
    // LHS derefence
    if(e->args[0]->optype == POINT_O)
    {
      // put underlying address on stack
      char *s = evalexpr(e->args[0]->args[0]);
      appmac(assem, s);
    }

    // LHS identifier
    else
    {
      // check that decl exists
      decl *dcl = e->args[0]->dcl;
      assert(dcl);

      // put location (address) of decl to stack
      char *s = pushlocat(dcl->locat);
      appmac(assem, s);
    }

    // // put LHS address in eax, deallocate
    // vspmac(assem, "mov eax, %s [esp]\n", sizenasm(PTR_SIZE));
    // sdall(PTR_SIZE);

    // put RHS value on stack
    char *s = evalexpr(e->args[1]);
    appmac(assem, s);

    char *rs = regstr(EBX, size);
    char *sizestr = sizenasm(size);

    // put RHS value in ebx, deallocate
    vspmac(assem, "mov %s, %s [esp]\n", rs, sizestr);
    sdall(size);

    // put LHS address in eax, deallocate
    vspmac(assem, "mov eax, %s [esp]\n", sizenasm(PTR_SIZE));
    sdall(PTR_SIZE);

    // transfer RHS value to LHS address
    vspmac(assem, "mov %s [eax], %s\n", sizestr, rs);

    // put value on stack as well
    sall(size);
    vspmac(assem, "mov %s [esp], %s\n", sizestr, rs);

    dbgstatus = "EQ_O";
  }
  // TODO rest of assignment

  // casts
  else if(ot == CAST_O)
  {
    ctype toct = e->ct;

    // size of original
    expr *from = e->args[0];
    ctype fromct = from->ct;
    int fromsize = safesizeoftype(fromct);

    // evaluate original, put on stack
    char *s = evalexpr(from);
    appmac(assem, s);

    // cast to void, discard value (dealloc)
    if(ctisdt(toct, VOID_T))
    {
      sdall(fromsize);
    }

    // do not discard value, go on with cast
    else
    {
      int tosize = sizeoftype(toct);
      
      // // move from stack to gpr a, deallocate
      // vspmac(assem, "mov %s, %s [esp]\n", regstr(EAX, fromsize), sizenasm(fromsize));
      // sdall(fromsize);
      
      // pointer to integral
      if(isptr(fromct) && isintegral(toct))
      {
        // replace fromct with this TM_DAT ctype
        typemod tm;
        tm.gen.type = TM_DAT;
        tm.dat.dt = PTRINT;

        // convert
        char *s = evalintcast(toct, &tm);
        appmac(assem, s);

        // // put on stack
        // sall(tosize);
        // vspmac(assem, "mov %s [esp], %s\n", sizenasm(tosize), regstr(EAX, tosize));
      }
      
      // integral to pointer
      else if(isintegral(fromct) && isptr(toct))
      {
        // replace toct with this TM_DAT ctype
        typemod tm;
        tm.gen.type = TM_DAT;
        tm.dat.dt = PTRINT;

        // convert
        char *s = evalintcast(&tm, fromct);
        appmac(assem, s);
        
        // // put on stack
        // sall(tosize);
        // vspmac(assem, "mov %s [esp], %s\n", sizenasm(tosize), regstr(EAX, tosize));
      }

      // pointer to pointer, function pointer to function pointer
      else if(isptr(fromct) && isptr(toct))
        ; // just do nothing; IA-32 does not mandate alignment, i think

      // integral to integral
      else if(isintegral(fromct) && isintegral(toct))
      {
        // convert
        char *s = evalintcast(toct, fromct);
        appmac(assem, s);

        // // put on stack
        // sall(tosize);
        // vspmac(assem, "mov %s [esp], %s\n", sizenasm(tosize), regstr(EAX, tosize));
      }

      else
      {
        throw("unanticipated cast");
      }

      // TODO floating point
    }
    dbgstatus = "CAST_O";
  }
  
  else if(ot == ADD_O || ot == SUB_O)
  {
    expr *e1 = e->args[0];
    expr *e2 = e->args[1];
    
    // if integral + pointer, switch order (pointer + integral)
    if(isptr(e2->ct) && ot == ADD_O)
    {
      expr *temp = e2;
      e2 = e1;
      e1 = temp;
    }

    ctype ct1 = e1->ct;
    ctype ct2 = e2->ct;

    // both arithmetic, just take the sum/difference
    if(isarith(ct1) && isarith(ct2))
    {
      assert(iscompat(ct1, ct2, QM_NOCARE)); // make sure same dattype

      int size = sizeoftype(ct1); // size of ints we're working with

      // both integral
      if(isintegral(ct1) && isintegral(ct2))
      {
        // evaluate args, put on stack
        appmac(assem, evalexpr(e1));
        appmac(assem, evalexpr(e2));

        char *op = (ot == SUB_O) ? "sub" : "add";

        char *rs = regstr(EAX, size);
        char *ss = sizenasm(size);
        // put e2 into gpr a. dealloc. add/subtract gpr a to e1 on the stack
        vspmac(assem, "mov %s, %s [esp]\n", rs, ss);
        sdall(size);
        vspmac(assem, "%s %s [esp], %s\n", op, ss, rs);
      }

      else
      {
        throw("non-integral arithmetic addition not supported yet");
      }
    }

    // pointer (+- integral)
    else if(isptr(ct1) && isintegral(ct2))
    {
      // only integers can be added to pointers
      assert(isintegral(ct2));
      assert(!ctisdt(ct1+1, VOID_T)); // can't do arithmetic on void pointers

      // size of target
      int targsize = sizeoftype(ct1+1);
      // size of integer
      int intsize = sizeoftype(ct2);

      // calculate pointer, put on stack
      appmac(assem, evalexpr(e1));

      // evaluate integer
      appmac(assem, evalexpr(e2));
      
      // put integer into gpr a, dealloc
      appmac(assem, stack2reg(EAX, intsize));
      sdall(intsize);
      
      // if not proper size, sign extend/zero extend so that multiplication works
      char *rs1 = regstr(EAX, dtsize(PTRINT));
      char *rs2 = regstr(EAX, intsize);
      if(issigned(ct2) && intsize < PTR_SIZE)
      {
        vspmac(assem, "movsx %s, %s\n", rs1, rs2);
      }
      else if(!issigned(ct2) && intsize < PTR_SIZE)
      {
        vspmac(assem, "movzx %s, %s\n", rs1, rs2);
      }

      // multiply offset by targsize
      char *rb = regstr(EBX, PTR_SIZE); // gpr b
      vspmac(assem, "mov %s, %s\n", rb, num2str(targsize)); // gpr b <- targsize
      vspmac(assem, "mul %s\n", rb); // gpr a <- gpr a * gpr b
      // imul and mul do not matter here because we assume overflow does not occur.
      // overflow does not occur because the difference between any two addresses fits within eax

      char *op = (ot == SUB_O) ? "sub" : "add";
      // add offset to pointer
      vspmac(assem, "%s %s [esp], %s\n", op, sizenasm(PTR_SIZE), regstr(EAX, PTR_SIZE));
    }

    // both pointers
    else if(isptr(ct1) && isptr(ct2))
    {
      assert(ot == SUB_O); // can't add pointers to each other, only subtract

      // can't subtract pointers of different types
      assert(iscompat(ct1, ct2, QM_NOCARE));
      assert(!ctisdt(ct1+1, VOID_T)); // can't subtract void pointers

      int targsize = sizeoftype(ct1+1);

      // evaluate both pointers onto stack
      appmac(assem, evalexpr(e1));
      appmac(assem, evalexpr(e2));

      // put second pointer into gpr b
      appmac(assem, stack2reg(EBX, PTR_SIZE));
      sdall(PTR_SIZE);
      // put first pointer into gpr a
      appmac(assem, stack2reg(EAX, PTR_SIZE));
      sdall(PTR_SIZE);

      // subtract second from first
      vspmac(assem, "sub %s, %s\n", regstr(EAX, PTR_SIZE), regstr(EBX, PTR_SIZE));
      
      // divide eax by target size
      appmac(assem, "cdq\n"); // sign extend into EDX:EAX
      vspmac(assem, "mov %s, %s\n", regstr(EBX, PTR_SIZE), num2str(targsize));
      vspmac(assem, "idiv %s\n", regstr(EBX, PTR_SIZE)); // perform signed division, eax/ebx -> edx:eax

      // put back onto stack
      sall(PTR_SIZE);
      appmac(assem, reg2stack(EAX, PTR_SIZE));
    }

    else
    {
      throw("unanticipated addition types");
    }

    dbgstatus = "ADD_O";
  }

  // multiply
  else if(ot == MULT_O)
  {
    expr *e1 = e->args[0];
    expr *e2 = e->args[1];
    
    // get size
    assert(sizeoftype(e1->ct) == sizeoftype(e2->ct));
    int size = sizeoftype(e1->ct);

    // eval 2 subexprs
    appmac(assem, evalexpr(e->args[0]));
    appmac(assem, evalexpr(e->args[1]));

    // put into regs
    appmac(assem, stack2reg(EBX, size));
    sdall(size);
    appmac(assem, stack2reg(EAX, size));
    sdall(size);
    
    // multiply
    vspmac(assem, "mul %s\n", regstr(EBX, size));
    // both mul and imul work here as we don't care about the upper half
    
    // put back onto stack
    sall(size);
    appmac(assem, reg2stack(EAX, size));
    
    dbgstatus = "MULT_O";
  }
  
  // division, modulo
  else if(ot == DIV_O || ot == MOD_O)
  {
    expr *e1 = e->args[0];
    expr *e2 = e->args[1];
    
    // get size
    assert(sizeoftype(e1->ct) == sizeoftype(e2->ct));
    int size = sizeoftype(e1->ct);

    // eval 2 subexprs
    appmac(assem, evalexpr(e->args[0]));
    appmac(assem, evalexpr(e->args[1]));
    
    // put into regs
    appmac(assem, stack2reg(EBX, size));
    sdall(size);
    appmac(assem, stack2reg(EAX, size));

    // if dividing, discard e1. otherwise it is needed for modulo later
    if(ot == DIV_O)
    {
      sdall(size);
    }

    // the following code assumes the integers are currently 32-bit
    
    // if signed, sign extend into EDX
    if(issigned(e1->ct))
    {
      appmac(assem, "cdq\n");
    }
    else // otherwise, fill EDX with 0
    {
      appmac(assem, "mov edx, 0\n");
    }
    
    vspmac(assem, "idiv ebx\n"); // perform signed division, eax/ebx -> edx:eax. discard edx.
    
    // division
    // return quotient
    if(ot == DIV_O)
    {
      sall(size);
      appmac(assem, reg2stack(EAX, size));

      dbgstatus = "DIV_O";
    }

    // modulo
    // a % b == a - (a/b)*b
    else
    {
      // (a/b) * b -> gpr a
      appmac(assem, "mul ebx\n");

      // a - (a/b)*b (a is on stack already)
      vspmac(assem, "sub %s [esp], %s\n", sizenasm(size), regstr(EAX, size));

      dbgstatus = "MOD_O";
    }
  }
  
  // unary minus
  else if(ot == UMIN_O)
  {
    appmac(assem, evalexpr(e->args[0]));
    vspmac(assem, "neg %s [esp]\n", sizenasm(size));

    dbgstatus = "UMIN_O";
  }

  // unary plus
  else if(ot == UPLUS_O)
  {
    // doesn't have any effect
    appmac(assem, evalexpr(e->args[0]));

    dbgstatus = "UPLUS_O";
  }

  // bitwise not
  else if(ot == BNOT_O)
  {
    appmac(assem, evalexpr(e->args[0]));
    vspmac(assem, "not %s [esp]\n", sizenasm(size));

    dbgstatus = "BNOT_O";
  }

  // bitwise binary operators
  else if(ot == BAND_O || ot == XOR_O || ot == BOR_O)
  {
    char *opstr;
    if(ot == BAND_O) opstr = "and";
    if(ot == XOR_O) opstr = "xor";
    if(ot == BOR_O) opstr = "or";

    // eval 2 subexprs, put on stack
    appmac(assem, evalexpr(e->args[0]));
    appmac(assem, evalexpr(e->args[1]));

    // put 2nd subexpr into eax, deallocate
    vspmac(assem, "mov %s, %s [esp]\n", regstr(EAX, size), sizenasm(size));
    sdall(size);
    
    // combine two operands on stack
    vspmac(assem, "%s %s [esp], %s\n", opstr, sizenasm(size), regstr(EAX, size));
  }

  // logical not
  else if(ot == LNOT_O)
  {
    // it's possible to do this without any conditionals but it's very long

    appmac(assem, evalexpr(e->args[0]));

    char *lab1 = newloclab();
    char *lab2 = newloclab();
    
    // test if 0
    vspmac(assem, "test %s [esp], -1\n", sizenasm(size));
    vspmac(assem, "jz %s\n", lab1);
    vspmac(assem, "mov %s [esp], 0\n", sizenasm(size));
    vspmac(assem, "jmp %s\n", lab2);
    vspmac(assem, "%s: mov %s [esp], 1\n", lab1, sizenasm(size));
    vspmac(assem, "%s:\n", lab2);

    dbgstatus = "LNOT_O";
  }

  // relational operators
  else if
    (ot == LT_O
     || ot == GT_O
     || ot == LEQ_O
     || ot == GEQ_O
     || ot == EQEQ_O
     || ot == NEQ_O)
    {
      expr *e1 = e->args[0];
      expr *e2 = e->args[1];

      // get size
      assert(sizeoftype(e1->ct) == sizeoftype(e2->ct));
      int argsize = sizeoftype(e1->ct);

      // eval 2 subexprs
      appmac(assem, evalexpr(e->args[0]));
      appmac(assem, evalexpr(e->args[1]));

      // put into regs
      appmac(assem, stack2reg(EBX, argsize));
      sdall(argsize);
      appmac(assem, stack2reg(EAX, argsize));
      sdall(argsize);


      // whether or not is signed
      int sgnd;

      // comparing two pointers (same as unsigned ints)
      if(isptr(e1->ct) && isptr(e2->ct))
      {
        sgnd = 0;
      }
      else if(issigned(e1->ct) && issigned(e2->ct))
      {
        sgnd = 1;
      }
      else if(!issigned(e1->ct) && !issigned(e2->ct))
      {
        sgnd = 0;
      }
      else
      {
        throw("impossible state");
      }

      // column 0 for unsigned, column 1 for signed
      static char *jumps[][2] = {
        [EQEQ_O] = {"je"  , "je"}  , 
        [NEQ_O]  = {"jne" , "jne"} , 
        [LT_O]   = {"jb"  , "jl"}  , 
        [GT_O]   = {"ja"  , "jg"}  , 
        [LEQ_O]  = {"jbe" , "jle"} , 
        [GEQ_O]  = {"jae" , "jge"} , 
      };

      // string used to jump (je, ja, jab, jge, etc.)
      char *jmpstr = jumps[ot][sgnd];

      // ff(jmpstr);
      // create 2 new labels to jump to
      char *lab1 = newloclab();
      char *lab2 = newloclab();

      /* e.x. for (a == b)

        cmp eax, ebx
        je .lab1
        mov dword [esp], 0
        jmp .lab2
        .lab1:
        mov dword [esp], 1
        .lab2:

      */
      // allocate space
      sall(size);

      // compare the two
      vspmac(assem, "cmp %s, %s\n", regstr(EAX, argsize), regstr(EBX, argsize));

      vspmac(assem, "%s %s\n", jmpstr, lab1);
      vspmac(assem, "mov %s [esp], 0\n", sizenasm(size));
      vspmac(assem, "jmp %s\n", lab2);
      vspmac(assem, "%s: mov %s [esp], 1\n", lab1, sizenasm(size));
      vspmac(assem, "%s:\n", lab2);

      dbgstatus = "RELAT_E";
    }

  // postfix increment-decrement
  // (return value, then increment)
  else if(ot == POSTINC_O || ot == POSTDEC_O
      || ot == PREINC_O || ot == PREDEC_O)
  {
    int ptr = isptr(e->args[0]->ct);
    int targsize;
    if(ptr)
      targsize = sizeoftype(e->args[0]->ct + 1);
    
    char *opstr;
    if(ot == POSTINC_O || ot == PREINC_O)
    {
      opstr = ptr ? "add" : "inc";
    }
    else
    {
      opstr = ptr ? "sub" : "dec";
    }

    
    // dereference
    if(e->args[0]->optype == POINT_O)
    {
      // put underlying address on stack
      char *s = evalexpr(e->args[0]->args[0]);
      appmac(assem, s);
    }

    // identifier
    else
    {
      // check that decl exists
      decl *dcl = e->args[0]->dcl;
      assert(dcl);

      // put location (address) of decl to stack
      char *s = pushlocat(dcl->locat);
      appmac(assem, s);
    }

    // put address into eax
    appmac(assem, stack2reg(EAX, PTR_SIZE));
    sdall(PTR_SIZE);

    // inc/dec original value before putting on stack
    if(ot == PREINC_O || ot == PREDEC_O)
    {
      if(isptr(e->args[0]->ct))
      {
        char *sizestr = num2str(targsize);
        vspmac(assem, "%s %s [%s], %s\n", opstr, sizenasm(size), regstr(EAX, PTR_SIZE), sizestr);
      }
      else
      {
        vspmac(assem, "%s %s [%s]\n", opstr, sizenasm(size), regstr(EAX, PTR_SIZE));
      }
    }
      
    // put old value onto stack
    sall(size);
    vspmac(assem, "mov %s, %s [%s]\n", regstr(EBX, size), sizenasm(size), regstr(EAX, PTR_SIZE)); // put into ebx
    vspmac(assem, "mov %s [esp], %s\n", sizenasm(size), regstr(EBX, size));

    // inc/dec original value after putting on stack
    if(ot == POSTINC_O || ot == POSTDEC_O)
    {
      if(isptr(e->args[0]->ct))
      {
        char *sizestr = num2str(targsize);
        vspmac(assem, "%s %s [%s], %s\n", opstr, sizenasm(size), regstr(EAX, PTR_SIZE), sizestr);
      }
      else
      {
        vspmac(assem, "%s %s [%s]\n", opstr, sizenasm(size), regstr(EAX, PTR_SIZE));
      }
    }


    dbgstatus = "INC/DEC";
  }

  // shifts
  else if(ot == SHL_O || ot == SHR_O)
  {
    char *opstr = (ot == SHL_O) ? "shl" : "shr";

    // evaluate lhs, put on stack
    appmac(assem, evalexpr(e->args[0]));

    // evaluate rhs, put on stack
    appmac(assem, evalexpr(e->args[1]));
    
    // put rhs into cl (truncate if necessary)
    appmac(assem, "mov cl, byte [esp]\n");
    sdall(sizeoftype(e->args[1]->ct)); // deallocate

    vspmac(assem, "%s %s [esp], cl\n", opstr, sizenasm(size));
  }

  // logical and/or
  else if(ot == LAND_O || ot == LOR_O)
  {
    // sizes can be different
    int ss1 = sizeoftype(e->args[0]->ct);
    int ss2 = sizeoftype(e->args[1]->ct);

    // allocate space for result at beginning
    sall(size);

    // evaluate first subexpr (don't evaluate second yet)
    appmac(assem, evalexpr(e->args[0]));

    // put into EAX
    appmac(assem, stack2reg(EAX, ss1));
    sdall(ss1);

    char *lab1 = newloclab();
    char *lab2 = newloclab();

    // jump conditional for short circuiting
    char *shortjmp = (ot == LOR_O) ? "jnz" : "jz";

    // short-circuit result, regular result (opposites)
    char *shortres = (ot == LOR_O) ? "1" : "0";
    char *regres = (ot == LOR_O) ? "0" : "1";
    
    vspmac(assem, "test %s, %s\n", regstr(EAX, ss1), regstr(EAX, ss1));
    vspmac(assem, "%s %s\n", shortjmp, lab1); // if nonzero, short-circuit to lab1

    // else, eval and test second expr
    appmac(assem, evalexpr(e->args[1]));
    appmac(assem, stack2reg(EAX, ss2));
    sdall(ss2); // the stacksize safety system still works here, because the allocation and deallocation are both in the same branch (no short circuit)
    vspmac(assem, "test %s, %s\n", regstr(EAX, ss2), regstr(EAX, ss2));
    vspmac(assem, "%s %s\n", shortjmp, lab1); // if nonzero, go to lab1

    // return regular result (passed both tests)
    vspmac(assem, "mov %s [esp], %s\n", sizenasm(size), regres);
    vspmac(assem, "jmp %s\n", lab2); // go to lab2

    // lab1: return short circuit result (failed a test)
    vspmac(assem, "%s: mov %s [esp], %s\n", lab1, sizenasm(size), shortres);
    vspmac(assem, "%s:\n", lab2);
  }

  else if(ot == TERN_O)
  {
    // evaluate 1st subexpr
    appmac(assem, evalexpr(e->args[0]));
    int ss1 = sizeoftype(e->args[0]->ct);

    assert(sizeoftype(e->args[1]->ct) == sizeoftype(e->args[2]->ct)); // check that LHS and RHS are same size
    int ss2 = sizeoftype(e->args[1]->ct);
    
    // put in eax, dealloc
    appmac(assem, stack2reg(EAX, ss1));
    sdall(ss1);

    // create labels
    char *lab1 = newloclab();
    char *lab2 = newloclab();
    
    // test
    vspmac(assem, "test %s, %s\n", regstr(EAX, ss1), regstr(EAX, ss1));

    // if 0, go to lab1
    vspmac(assem, "jz %s\n", lab1);
    
    // otherwise evaluate LHS
    appmac(assem, evalexpr(e->args[1]));
    vspmac(assem, "jmp %s\n", lab2);
    
    // lab1, evaluate LHS
    vspmac(assem, "%s:\n", lab1);
    appmac(assem, evalexpr(e->args[2]));
    vspmac(assem, "%s:\n", lab2);

    // each branch allocates ss2 onto the stack by evaluating, but only one runs in reality. therefore the stacksize safety system fails here and needs to be corrected
    stacksize -= ss2;
  }

  // comma operator
  else if(ot == COMMA_O)
  {
    // evaluate first expr (order matters)
    appmac(assem, evalexpr(e->args[0]));
    // discard result
    sdall(sizeoftype(e->args[0]->ct));

    // evaluate second expr, leave result on stack
    appmac(assem, evalexpr(e->args[1]));
  }

  // function call!!!
  else if(ot == FUN_O)
  {
    int argssize = 0; // record size of args on stack
    
    // eval, push args to stack in reverse order (to match up with the calculated offsets in proctoplevel())
    expr *arglist = e->args[1];
    for(int i = arglist->numargs - 1; i >= 0; i--)
    {
      appmac(assem, evalexpr(arglist->args[i]));
      argssize += sizeoftype(arglist->args[i]->ct);
    }

    // get function pointer
    appmac(assem, evalexpr(e->args[0]));
    // put into eax, dealloc
    vspmac(assem, "mov %s, %s [esp]\n", regstr(EAX, PTR_SIZE), sizenasm(PTR_SIZE));
    sdall(PTR_SIZE);

    // call function
    vspmac(assem, "call %s\n", regstr(EAX, PTR_SIZE));
    // function creates stack frame, runs, destroys stack frame, leaves return value in EAX

    // deallocate args
    sdall(argssize);

    // if not void, put return type on stack
    if(size != 0)
    {
      sall(size);
      appmac(assem, reg2stack(EAX, size));
    }
  }

  else
  {
    puts(hropt[ot]);
    throw("optype does not exist/is not supported");
  }

  if(curstacksize + size != stacksize)
  {
    puts(dbgstatus);
    putd(curstacksize);
    putd(stacksize);
    putd(size);
    throw("allocations do not match deallocations");
  }
  
  // assert(curstacksize + size == stacksize);
  return assem;
}


//{{{1 main
int main()
{
  // TODO fix the literary hierarchy, it's still broken
  // should be doable with 1 run-through
  // TODO constantly print to stderr what token is being read, what line number, etc. so that when asserts fail it's immediately clear where it happened

  // errors = makelist(sizeof(char *));

  // TODO
  // assert(sizeof(float) == 4); // there is no int32_t analog for floats. we rely on the system the compiler is running on to make sure floats work
  // assert(sizeof(int) >= 4); // similar but less strict because longer ints are easy to convert to shorter

  // for marking quoted and escaped sections
  // char src[1000], quot[1000], esc[1000];
  alloc(char, src, srcsize, i);

  int c;

  while((c = getchar()) != EOF) // read in src
  {
    src[i++] = c;
    resize(src, srcsize, i);
  }
  src[i] = 0; // null terminate
  // printf("%s", src);
  // puts("-------------------");

  char *quot = malloc(i);
  char *esc = malloc(i);

  splice(src); // delete backslash + newline combinations

  mark_esc(src, esc); // mark backslash escape sequences
  mark_quot(src, esc, quot); // mark single and double quoted regions

  // rem_comments(src, esc, quot); // replace multiline comments with single space

  // // mark AGAIN now that comments are gone (stray quotes will break things
  // mark_esc(src, esc); // mark backslash escape sequences
  // mark_quot(src, esc, quot); // mark single and double quoted regions

  // stray_backslash(src, esc, quot); // check for stray backslashes, throw a tantrum if so
  check_stray(src, esc, quot, "#$@\\`"); // check for stray characters, throw a tantrum if so


  // showlithier(src, esc, quot);

  // we parse the top-level decls and function defs
  list *trans_unit = proctokens(src, esc, quot);
  token *toks = (token *) trans_unit->cont;

  // temporary, for testing
  int testexpr = 0;
  if(!testexpr)
  {
    proctoplevel(toks);
  }
  else
  {
    link *chain = tokl2ll((token *)trans_unit->cont, -1);
    
    expr *e = parseexpr(chain);
    dword d = evalconstintexpr(e);
    // putexpr(e);
    printf("%d\n", d);
  }

}
