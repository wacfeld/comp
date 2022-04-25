#include "defs.h"
#include "datastruct.h"
#include "is.h"

// list *dam = makelist(sizeof(void *)); // dynamically allocated memory, to be freed all at once later

// attach two links, accounting for null, creating temporary variables to account for yknow
#define attach(a, b) {link *ta = a; link *tb = b; if(ta) ta->right = tb; if(tb) tb->left = ta;}

// move to either end of a linked list
#define rightend(l) {while(l->right != NULL) l = l->right;}
#define leftend(l) {while(l->left != NULL) l = l->left;}
// it's not enough to just save either end of the list because the pointer might change

void puttok(token t);


#define RIGHT 1
#define LEFT 0


void sever(link *l)
{
  if(!l) return;
  assert(l->left);
  l->left->right = NULL;
  assert(l->right);
  l->right->left = NULL;
}


void etypeadd(expr *e, int type)
{
  if(e->type == NULL) 
    e->type = makeset(sizeof(int));
  setins(e->type, &type);
}

int eistype(expr *e, int type)
{
  if(e->type == NULL)
    return 0;

  else return inset(e->type, type);
}

int leistype(link *l, int type)
{
  return l != NULL && l->type == EXPR_L && eistype(l->cont.exp->type, type);
}


tok *ll2tokl(link *ll) // linked list to NOTOK-terminated token list
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
      tokl[i++] = *ll->cont.expr->tok;
    }
    ll = ll->right;
  }

  // NOTOK
  token nt;
  nt.gen.type = NOTOK;
  tokl[i] = nt;

  return tokl;
}

link *tokl2ll(token *tokl) // NOTOK-terminated token list to linked list
{
  int len = 0;
  while(!(tokl[len].gen.type == NOTOK)) len++;
  
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

void putll(link *l) // put linked list
{
  while(l != NULL)
  {
    if(l->type == TOK_L)
    {
      printf("TOKEN: ");
      puttok(*l->cont.tok);
    }

    if(l->type == EXPR_L)
    {
      printf("EXPR:  ");
      
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
  nline();
}


// gets next (toplevel) statement from string & tokenizes
// these two steps must be done simultaneously, as far as i can tell
// otherwise it's very difficult to tell when a statement ends
token *nextstat(char *src, char *esc, char *quot)
{
}


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

      t->integer.cont = num;

      return t;
    }

    else if(src[i] == '.') // float constant
    {
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

    assert(!"this assert should never run");
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

    char chr;
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

int iskeyword(token *t, enum keyword k)
{
  return t != NULL && t->gen.type == KEYWORD && t->keyword.cont == k;
}
int isatom(token *t, enum atom_type a)
{
  return t != NULL && t->gen.type == ATOM && t->atom.cont == a;
}

int lisop(link *l, int o)
{
  return l != NULL && l->type == EXPR_L && l->cont.exp->optype == o;
}
int lisexpr(link *l, expr_type e)
{
  return l != NULL && l->type == EXPR_L && l->cont.exp->type == e;
}
int lisatom(link *l, enum atom_type a)
{
  return l != NULL && l->type == TOK_L && isatom(l->cont.tok, a);
}

int listok(link *l, enum tok_type t)
{
  return l != NULL && l->type == TOK_L && l->cont.tok->gen.type == t;
}

// find matching paren, brace, matching ?:, etc.
link *findmatch(link *start, int dir, token inc, token dec)
{
  int dep = 1;
  link *curl = start;
  do
  {
    curl = (dir == RIGHT) ? curl->right : curl->left;

    if(listok(curl, inc)) dep++;
    if(listok(curl, dec)) dep--;
    assert(parendep >= 0 && curl != NULL);

  } while(parendep > 0);
  
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


int gettypespec(token t) // get type specifier
{
  if(t.gen.type == KEYWORD)
  {
    enum keyword k = t.keyword.cont;
    if(k == K_VOID || k == K_CHAR || k == K_SHORT || k == K_INT || k == K_LONG || k == K_FLOAT || k == K_DOUBLE || k == K_SIGNED || k == K_UNSIGNED)
    {
      return k;
    }
    else return -1;
  }
  // TODO user-defined specifiers, can be identifiers
  else return -1;

}

int gettypequal(token t) // get type qualifier
{
  if(t.gen.type == KEYWORD)
  {
    enum keyword k = t.keyword.cont;
    if(k == K_VOLATILE || k == K_CONST)
    {
      return k;
    }
    else return -1;
  }
  return -1;
}

int getstorespec(token t) // get storage class specifier
{
  if(t.gen.type == KEYWORD)
  {
    enum keyword k = t.keyword.cont;
    if(k == K_AUTO || k == K_REGISTER || k == K_STATIC || k == K_EXTERN || k == K_TYPEDEF)
    {
      return k;
    }
    return -1;
  }
  return -1;
}

int isdeclspec(token t) // get declaration specifier, -1 if it's not that
{
  int x;
  if((x = gettypespec(t)) != -1)
  {
    return x;
  }
  if((x = getstorespec(t)) != -1)
  {
    return x;
  }
  return gettypequal(t);
}

// recursive function that interprets a declarator by getting the type modifiers in order
// returns hi + 1 (where to pick up next)
int gettypemods(token *toks, int lo, int hi, list *l, int abs)
{
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

    if(toks[hi].gen.type == IDENT) // not abstract, has identifier
    {
      abs = 0;
      hi++; // pass over
    }
    else
    {
      abs = 1;
    }

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

  // get one typemod then recurse
  typemod *tmod = malloc(sizeof(typemod));

  if(isatom(toks+lo, STAR)) // pointer time
  {
    tmod->gen.type = TM_PTR;

    // look for qualifiers
    lo++;
    for(;; lo++)
    {
      if(iskeyword(toks+lo, K_VOLATILE))
      {
        tmod->ptr.isvolatile = 1;
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

    gettypemods(toks, lo, hi, l, abs);

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

        if(!fun) // enclosing
        {
          free(tmod); // wasn't needed
          // recurse without the paren
          gettypemods(toks, lo+1, hi-1, l, abs);

          return hi+1;
        }
      }

      if(fun) // it's a function
      {
        tmod->gen.type = TM_FUNC;
        // TODO store the parameter type list
        append(l, tmod);
        free(tmod);
        if(!abs || i != lo) gettypemods(toks, lo, i-1, l, abs); // must recurse further
        
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

      // TODO evaluate and store length (should be constant expression)
      tmod->gen.type = TM_ARR;
      append(l, tmod);
      free(tmod);
      // gettypemods(toks, lo, i-1, l, abs);
      // no need to recurse; we're done because it's abstract and the [] was at the start

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
    assert(isatom(toks+lo), BRACKOP);

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
    if(ts.ptr.isvolatile)
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

// get storespecs, typequals, and typespecs from the front of a declaration
ctype *getdeclspecs(token *toks, int *i)
{
  ctype *ct = malloc(sizeof(ctype));
  
  // allocate sets
  set *typespecs  = makeset(sizeof(int));
  set *typequals  = makeset(sizeof(int));
  set *storespecs = makeset(sizeof(int));

  // assign
  ct->typespecs = typespecs;
  ct->typequals = typequals;
  ct->storespecs = storespecs;
  ct->typemods = NULL;

  int n = *i; // save starting point for future reference (e.x. checking typedef)
  token t;
  int spec;
  // read declaration specifiers
  for(;; (*i)++)
  {
    t =toks[*i];
    if((spec = gettypespec(t)) != -1)
    {
      // insert
      assert(!setins(typespecs, &spec)); // no duplicate type specifiers allowed
    }
    else if((spec = gettypequal(t)) != -1)
    {
      setins(typequals, &spec); // duplicate type quals are ignored
    }
    else if((spec = getstorespec(t)) != -1)
    {
      assert(!setins(storespecs, &spec)); // no duplicate storage classes allowed
    }
    else break; // end of declaration specifiers
  }

  return ct;
}

// read first declaration from array of tokens, and do things about it
// returns NULL if runs into NOTOK
ctype * parsedecl(token *toks, int onlydecl)
{
  // declaration *decl = malloc(sizeof(decl));
  
  static int i = 0;

  if(!toks) // reset if passed NULL
  {
    i = 0;
    return NULL; // return value doesn't matter
  }

  if(toks[i].gen.type == NOTOK) // end of token stream
  {
    return NULL;
  }
  
  ctype *ct = getdeclspecs(toks, &i); // parse declaration specifiers and move i forward past them all
  
  // TODO perform checks on the specifiers to make sure they're allowed
  /*
    auto and register only in functions
    conflicting type specifiers
    only one storage class allowed
    typedef must be at beginning of declaration
    functions inside a function are extern, functions declared outside are static with external linkage
    etc.
    static objects/arrays must be initialized with constant expressions
    technically, list members must be constant expressions even if auto or register
     */

  int temp = K_TYPEDEF; // because of how sets are implemented we need an address
  if(inset(storespecs, &temp)) // special case
  {
    // TODO
  }

  // in the case of enum, struct, or union, the type is not done:

  // else if(struct or union specifier) TODO

  // else if(enum specifier) TODO

  // we now are left with a declarator-initialier list, or a function declarator along with its definition

  list *l = makelist(sizeof(typemod));
  i = gettypemods(toks, i, -1, l, 0); // parse one declarator and move i forward accordingly
  reverse(l); // typemods are parsed from the outside in, so now we flip that
  ct->typemods = l; // add to ct

  if(onlydecl) // only declaration => no initialization, function definition, etc.
  {
    return ct;
  }

  // print debugging
  // typemod *tms = (typemod *) l->cont;
  // int tmlen = l->n;
  // for(int j = 0; j < tmlen; j++)
  // {
  //   // puttypemod(tms[j]);
  // }
  // int *tss = (int *) typespecs->cont;
  // int tslen = typespecs->n;
  // for(int j = 0; j < tslen; j++)
  // {
  //   // printf("%s ", keywords[tss[j]]);
  // }
  // // nline();


  // ctype ct = {typespecs, typequals, storespecs, l};

  // TODO comma-separated declarations

  // only after first and only declarator: function definition
  if(isatom(toks+i, BRACEOP))
  {
    // TODO
    // check if valid function declarator
    puts("(function definition)");
    int bracedep = 0;
    do
    {
      if(isatom(toks+i, BRACEOP)) bracedep++;
      if(isatom(toks+i, BRACECL)) bracedep--;
      i++;
    } while(bracedep);
  }

  else if(isatom(toks+i, EQ)) // initialization
  {
    // TODO
    puts("(initialization)");
    while(!isatom(toks+i, SEMICOLON)) i++;
    i++;
  }

  else
  {
    assert(isatom(toks+i, SEMICOLON));
    i++; // move over semicolon
  }

  return ct; // success
}

int lisin(link *l, int num, token *tokl)
{
  for(int i = 0; i < num; i++)
  {
    if(lisatom(l, tokl[i])) return 1;
  }

  return 0;
}


// next top level link that passes the function det()
link *nexttoplevel(link *start, link *dir, int num, int *atoms)
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

    if(listok(curl, PARENOP) || listok(curl, BRACKOP) || listok(curl, BRACEOP) || listok(curl, QUESTION))
      dep++;
    if(listok(curl, PARENCL) || listok(curl, BRACKCL) || listok(curl, BRACECL) || listok(curl, COLON))
      dep--;
    assert(dep >= 0);

    // if(det(curl) && dep == 0) // passes test, return
    if(lisin(curl, num, atoms) && dep == 0)
      return curl;

    if(curl == NULL)
      break;
    curl = (dir == RIGHT) ? curl->right : curl->left;
  }

  return NULL;
}


expr *makeexpr(int type, int optype, int arglen, ...)
{
  expr *e = calloc(1, sizeof(expr));
  etypeadd(e, type);
  e->optype = optype;
  e->arglen = arglen;
  e->args = malloc(sizeof(expr) * arglen);

  va_list ap;
  va_start(ap, arglen);
  for(i = 0; i < arglen; i++)
  {
    e->args[i] = va_arg(ap, expr *);
  }
  va_end(ap);

  return e;
}

// REQUIREMENTS
// start can be anywhere in the linked list
// use rightend() and leftend() to get it where you want
// the left and right ends must be null terminated

expr *parseexpr(link *start)
{
  rightend(start);
  static int commaops[] = {COMMA};
  link *comma = nexttoplevel(start, LEFT, 1, commaops);
  assert(start != comma) // no empty subexprs
  
  if(!comma) // base case, drop down
  {
    return parseasgnexpr(start);
  }
  
  // recurse sideways
  
  // detach
  start->right = NULL; // can't hurt
  // comma->right->left = NULL;
  // comma->left->right = NULL;
  sever(comma);

  expr *e1 = parseexpr(comma->left);
  expr *e2 = parseasgnexpr(start);

  expr *newe = makeexpr(EXPR, COMMA, 2, e1, e2);
  return newe;
}

expr *parseasgnexpr(link *start)
{
  leftend(start);

  static int asgnops[] = {EQ, TIMESEQ, DIVEQ, MODEQ, PLUSEQ, MINEQ, SHLEQ, SHREQ, ANDEQ, XOREQ, OREQ};
  link *op = nexttoplevel(start, RIGHT, sizeof(asgnops)/sizeof(int), asgnops);
  assert(start != op); // no empty assignment
  
  if(!op)
  {
    return parsecondexpr(start);
  }
  
  start->left = NULL;
  // op->right->left = NULL;
  // op->left->right = NULL;
  sever(op);

  expr *e1 = parsecastunaryexpr(start);
  expr *e2 = parseasgnexpr(op->right);

  expr *newe = makeexpr(ASGN_E, op->cont.tok->atom.type, 2, e1, e2);
  return newe;
}

expr *parsecondexpr(link *start)
{
  leftend(start);

  link *quest = nexttoplevel(start, RIGHT, 1, QUESTION);

  assert(start != quest);

  if(!quest)
    return parselorexpr(start);

  link *colon = findmatch(quest, RIGHT, QUESTION, COLON);
  assert(quest + 1 != colon);

  sever(quest);
  sever(colon);

  expr *e1 = parselorexpr(quest->left);
  expr *e2 = parseexpr(quest->right);
  expr *e3 = parsecondexpr(colon->right);

  expr *newe = makeexpr(COND_E, TERN_O, 3, e1, e2, e3);
  return newe;
}

// parse LTR binary expression, a very common expression type, which can be generalized
expr * parseltrbinexpr(link *start, int etype, int num, int *atoms, int *optypes, expr *(*down)(link *))
{
  rightend(start);
  link *op = nexttoplevel(start, LEFT, num, atoms);

  assert(start != op);

  if(!op)
    return down(start);

  int atom = op->cont.tok->atom.type;
  int optype;
  for(int i = 0; i < num; i++)
  {
    if(atoms[i] == atom)
    {
      optype = optypes[i];
    }
  }


  start->right = NULL;
  sever(op);
  expr *e1 = parseltrbinexpr(op->left, etype, atoms, optypes, down); // recurse sideways
  expr *e2 = down(start);
  
  expr *newe = makeexpr(etype, optype, 2, e1, e2);
  return newe;
}

expr *parselorexpr(link *start)
{
  static int at[] = {LOGOR};
  static int op[] = {LOR_O};
  return parseltrbinexpr(start, LOR_E, 1, at, op, parselandexpr);
}

expr *parselandexpr(link *start)
{
  static int at[] = {LOGAND};
  static int op[] = {LAND_O};
  return parseltrbinexpr(start, LAND_E, 1, at, op, parseorexpr);
}

expr *parseorexpr(link *start)
{
  static int at[] = {BITOR};
  static int op[] = {BOR_O};
  return parseltrbinexpr(start, OR_E, 1, at, op, parsexorexpr);
}

expr *parsexorexpr(link *start)
{
  static int at[] = {XOR};
  static int op[] = {XOR_O};
  return parseltrbinexpr(start, XOR_E, 1, at, op, parseandexpr);
}

expr *parseandexpr(link *start)
{
  static int at[] = {BITAND};
  static int op[] = {BAND_O};
  return parseltrbinexpr(start, AND_E, 1, at, op, parseeqexpr);
}

expr *parseeqexpr(link *start)
{
  static int at[] = {EQEQ, NOTEQ};
  static int op[] = {EQEQ_O, NEQ_O};
  return parseltrbinexpr(start, EQUAL_E, 2, at, op, parserelexpr);
}

expr *parserelexpr(link *start)
{
  static int at[] = {LESS, GREAT, LEQ, GEQ};
  static int op[] = {LT_O, GT_O, LEQ_O, GEQ_O};
  return parseltrbinexpr(start, RELAT_E, 4, at, op, parseshiftexpr);
}

expr *parseshiftexpr(link *start)
{
  static int at[] = {SHL, SHR};
  static int op[] = {SHL_O, SHR_O};
  return parseltrbinexpr(start, SHIFT_E, at, op, parseaddexpr);
}

expr *parseaddexpr(link *start)
{
  static int at[] = {PLUS, MIN};
  static int op[] = {ADD_O, SUB_O};
  return parseltrbinexpr(start, ADD_E, at, op, parsemultexpr);
}

expr *parsemultexpr(link *start)
{
  static int at[] = {STAR, DIV, MOD};
  static int op[] = {MULT_O, DIV_O, MOD_O};
  return parseltrbinexpr(start, MULT_E, at, op, parsecastunaryexpr);
}

// evaluate primary expressions
// this also parses top-level type names because we have to do that somewhere
link *parseprimexpr(link *chain)
{
  // chain is a linked list of tokens and expressions
  // ultimately the goal is to turn everything into a single expression
  link* curl = chain;

  while(curl != NULL)
  {
    // printf("%p\n", curl);
    // (expression)
    if(lisatom(curl, PARENOP))
    {
      if(lisatom(curl->right, PARENCL)) // empty parens -> funcall, ignore for now, deal with later as special case
      {
        curl = curl->right->right;
        continue;
      }
        
      // putd(0);
      // find matching
      // int parendep = 1;
      // link *parr = curl->right; // paren right
      // while(parendep > 0) // find opposite
      // {
      //   if(lisatom(parr, PARENOP)) parendep++;
      //   if(lisatom(parr, PARENCL)) parendep--;
      //   assert(parendep >= 0 && parr != NULL);
      //   parr = parr->right;
      // }
      link *parr = findmatch(curl, RIGHT, PARENOP, PARENCL);

      // parr = parr->left; // move onto )

      // detach
      curl->right->left = NULL;
      parr->left->right = NULL;

      if(isdeclspec(*curl->cont.tok)) // it's a cast, therefore not a primary expression
      {
        // convert to token list
        token *abstype = ll2tokl(curl->right);
        // parse type
        ctype *ct = parsedecl(abstype, 1);
        
        // put into expression
        expr *newe = calloc(1,sizeof(expr));
        // newe->type = TYPENAME;
        etypeadd(newe, TYPENAME);
        // no optype, or other things, only ct
        newe->ct = ct;

        // remove the parens. type-name can only ever occur within exactly one pair of parens, so this simplifies things
        // reattach
        attach(curl->left, newe);
        attach(newe, parr->right);
        curl = newe; // recentre

        // curl = opl; // move over
        continue;
      }

      // otherwise it's an expression or a function call
      // either way, it's a comma-separated list of assignment expressions, possibly empty if function call

      // disconnect
      // link *left = curl->left->left; // just outside the parens
      // link *right = opl->right->right;
      // curl->left = NULL;
      // opl->right = NULL;

      // evaluate
      link *l = parseexpr(curl->right);
      // encapsulate it in a primary expression (this is important!)
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,PRIM_E);
      newe->optype = PAREN_O;
      newe->args = calloc(1,sizeof(expr));
      newe->args[0] = l->cont.exp;
      l->cont.exp = newe;

      // write
      // link *l = malloc(sizeof(link));
      // l->type = EXPR_L;
      // l->cont.exp = exp;

      // reconnect
      attach(curl->left, l);
      attach(l, parr->right);
      // l->left = left;
      // l->right = right;
      // if(left) left->right = l;
      // if(right) right->left = l;
      // TODO free all those tokens, either inside the recursive call, or here
      // solution: free all at the end, from the translation unit. let them linger until then

      // reposition
      curl = l;
    }

    // constants
    else if(listok(curl, INTEGER)) // integer constant
    {
    // putd(1);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = INT_O;

      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }
    else if(listok(curl, CHAR))
    {
// putd(2);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = CHAR_O;

      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }
    else if(listok(curl, FLOATING))
    {
// putd(3);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = FLOAT_O;
      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }

    else if(listok(curl, STRLIT)) // string literal
    {
    // putd(4);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = STRING_O;
      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }

    else if(listok(curl, IDENT)) // identifiers
    {
    // putd(5);
      expr *e = calloc(1,sizeof(expr));
      etypeadd(e,PRIM_E);
      e->optype = IDENT_O;
      e->tok = curl->cont.tok;

      curl->type = EXPR_L;
      curl->cont.exp = e;
    }
    // otherwise it's not a primary expression

    // else putd(6);
    // move on to next link
    if(curl->right == NULL) break; // break preemptively, therefore this while loop could be a while(1) but anyway, it's so we don't get left with NULL
    curl = curl->right;
  }

  // find the leftmost link again, to return
  // due to the recursive (expression) parsing, chain may no longer point to it
  while(curl->left != NULL)
  {
    // printf("%p\n", curl);
    curl = curl->left;
  }
  return curl;
}


// postfix expression
link *parsepostexpr(link *chain)
{
  // parse primary expressions
  chain = parseprimexpr(chain);
  // promote all primary expressions to postfix
  link *curl = chain;
  while(curl != NULL)
  {
    if(leistype(curl, PRIM_E))
    {
      etypeadd(curl->cont.exp,POST_E);
    }
    curl = curl->right;
  }
  
  curl = chain; // back to start
  do // fold in post expressions until none left
  {
    // puts("----");
    // putll(curl);
    if(!leistype(curl, POST_E)) // if not postfix expression, pass over
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, BRACKOP)) // postfix-expression[expression]
    {
      // link* indr = curl->right; // index right
      link *indl = curl->right->right; // [

      // int brackdep = 1; // find matching

//       while(brackdep)
//       {
//         indr = indr->right;
//         if(lisatom(indr, BRACKOP)) brackdep++;
//         if(lisatom(indr, BRACKCL)) brackdep--;
//         assert(brackdep >= 0 && indr != NULL);
//       }
//       indr = indr->left; // left of ]
      link *indr = findmatch(curl->right, RIGHT, BRACKOP, BRACKCL);
      indr = indr->left; // left of ]

      link *outr = indr->right->right; // right of ]
      // detach
      indr->right = NULL;
      indl->left = NULL;

      // parse index expression
      indl = parseexpr(indl);
      expr *e = indl->cont.exp; // extract expression from link

      // make new expression from base and index
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E); // postfix
      newe->optype = ARR_O; // array indexing
      newe->args = malloc(sizeof(expr) * 2); // 2 args
      newe->args[0] = curl->cont.exp; // base
      newe->args[1] = e; // index

      // replace with new expression
      curl->cont.exp = newe;
      // reattach
      // curl->right = outr;
      // outr->left = curl;
      attach(curl, outr);
      
      // curl = chain; // restart since everything's been modified
      // we can probably not restart from the beginning, due to how postfix works, but this also works fine
    }

    else if(lisop(curl->right, PAREN_O)) // function call time
    {
      // TODO
    }

    else if(lisatom(curl->right, PARENOP)) // postfix-expression(argument-expression-list_opt)
    {
      // parens are a special case
      // paren tokens can only be (casts) and empty() now

      if(isdeclspec(*curl->cont.tok)) // cast, move over
      {
        // int parendep = 1;
        // curl = curl->right;
        // while(parendep)
        // {
        //   if(lisatom(curl), PARENOP) parendep++;
        //   if(lisatom(curl), PARENCL) parendep--;
        //   assert(parendep >= 0 && curl != NULL);
        //   curl = curl->right;
        // }

        curl = findmatch(curl, RIGHT, PARENOP, PARENCL)->right;
        continue;
      }

      if(lisatom(curl->right, PARENCL)) // empty args
      {
        // TODO
        curl = curl->right->right;
        continue;
      }

      // link *argl = curl->right->right; // left of arguments
      // link *argr = curl->right->right;
      
      // expr *newe = malloc(sizeof(expr));
      // newe->type = POST_E;
      // newe->optype = FUN_O;
      // newe->arglen = 0;
      // int size = 5;
      // newe->args = malloc(sizeof(expr) * size);

      // newe->args[0] = curl->cont.exp; // function name is the first thing
      // newe->arglen++;

      // int parendep = 1;
      // while(parendep) // while in argument parentheses
      // {
      //   if(lisatom(argr, PARENOP)) parendep++;
      //   if(lisatom(argr, PARENCL)) parendep--;
      //   assert(parendep >= 0 && argr != NULL);

      //   if((lisatom(argr, COMMA) && parendep == 1) || parendep == 0) // end of argument, evaluate
      //   {
      //     // detach
      //     argr->left->right = NULL;
      //     argl->left = NULL;

      //     // parse assignment expression
      //     argl = parseasgnexpr(argl);
      //     expr *e = argl->cont.exp;

      //     // append to argument list
      //     resize(newe->args, size, newe->arglen);
      //     newe->args[newe->arglen] = e;
      //     newe->arglen++;
          
      //     // setup for next argument
      //     argl = argr->right;
      //     argr = argr->right;
      //   }

      //   else
      //     argr = argr->right;

      // }

      // wrap up, reattach
      // curl->cont.exp = newe;
      // attach(curl, argr);
      // // curl->right = argr;
      // // argr->left = curl;
      
      // curl = chain; // restart
    }

    else if(lisatom(curl->right, DOT))
    {
      assert(listok(curl->right->right, IDENT)); // struct members are always single identifiers

      // create new expression
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = STRUCT_O;
      newe->args = malloc(sizeof(expr) * 2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      // attach
      curl->cont.exp = newe;
      // link *temp = curl->right->right->right;
      attach(curl, curl->right->right->right);
      // curl->right = curl->right->right->right;
      // curl->right->left = curl;

      // go back to beginning
      // curl = chain;
    }

    else if(lisatom(curl->right, ARROW))
    {
      assert(lisop(curl->right->right, IDENT_O));
      
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = PSTRUCT_O;
      newe->args = malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;
      // link *temp = curl->right->right->right;
      attach(curl, curl->right->right->right);

      // curl = chain;
    }

    else if(lisatom(curl->right, INC))
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = POSTINC_O;
      newe->args = malloc(sizeof(expr));
      newe->args[0] = curl->cont.exp;

      curl->cont.exp = newe;
      // link *temp = curl->right->right;
      attach(curl, curl->right->right);

      // curl = chain;
    }

    else if(lisatom(curl->right, DEC))
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,POST_E);
      newe->optype = POSTDEC_O;
      newe->args = malloc(sizeof(expr));
      newe->args[0] = curl->cont.exp;

      curl->cont.exp = newe;
      // link *temp = curl->right->right;
      attach(curl, curl->right->right);

      // curl = chain;
    }

    else
    {
      if(curl->right == NULL) break; // break preemptively
      curl = curl->right; // move on
    }
  } while(curl != NULL);


  // find start again
  while(curl->left != NULL)
  {
    // printf("%p\n", curl);
    curl = curl->left;
  }
  return curl;
  
}

// unary and cast expressions mutually reference each other so they have to be parsed together
link *parsecastunaryexpr(link *chain)
{
  // parse post
  chain = parsepostexpr(chain);

  int modified;
  link *curl = chain;

  do // we go back and forth between unary and cast, until modifications stop being made
  {
    modified = 0;

    // promote post to unary
    // go to left
    // while(curl->left != NULL)
    // {
    //   curl = curl->left;
    // }
    leftend(curl);

    link *temp = curl;
    while(temp != NULL)
    {
      if(leistype(temp, POST_E))
      {
        etypeadd(temp->cont.exp,UNAR_E);
      }
      temp = temp->right;
    }

    // navigate to end, because we're parsing right to left
    // while(curl->right != NULL)
    // {
    //   curl = curl->right;
    // }
    rightend(curl);

    // unary expressions
    do
    {
      // if(!leistype(curl, UNAR_E)) // if not unary expression, skip
      // {
      //   curl = curl->left;
      //   continue;
      // }

      if(leistype(curl, UNAR_E) && lisatom(curl->left, INC)) // ++ unary-expr
      {
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,UNAR_E);
        newe->optype = PREINC_O;
        newe->args = malloc(sizeof(expr));
        newe->args[0] = curl->cont.exp;

        curl->cont.exp = newe;
        // link *temp = curl->left->left;
        attach(temp, curl->left->left);

        modified = 1;
      }

      else if(leistype(curl, UNAR_E) && lisatom(curl->left, DEC)) // -- unary-expr
      {
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,UNAR_E);
        newe->optype = PREDEC_O;

        newe->args = malloc(sizeof(expr));
        newe->args[0] = curl->cont.exp;

        curl->cont.exp = newe;
        // link *temp = curl->left->left;
        attach(temp, curl->left->left);

        modified = 1;
      }

      // unary-operator cast-expression
      else if(leistype(curl, CAST_E) && lisunaryop(curl->left))
      {
        // get operator
        int atom = curl->left->cont.tok.atom.type;

        // create new expression
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,UNAR_E);

        int optype;
        if(atom      == BITAND) optype = ADDR_O;
        else if(atom == STAR) optype   = POINT_O;
        else if(atom == PLUS) optype   = UPLUS_O;
        else if(atom == MIN) optype    = UMIN_O;
        else if(atom == BITNOT) optype = BNOT_O;
        else if(atom == LOGNOT) optype = LNOT_O;

        newe->optype = optype;
        newe->args = malloc(sizeof(expr)); // unary obviously has 1 arg
        newe->args[0] = curl->cont.exp;
        curl->cont.exp = newe;

        // reattach
        attach(curl->left->left, curl);

        // signal modified
        modified = 1;

        // we could do curl = curl->left here because it's now a UNAR_E but it's not necessary
      }

      // else if(lisatom(curl->left, PARENCL)) // cast
      // {
      //   int parendep = 1;
      //   link *opparen = curl->left;

      //   while(parendep > 0 && opparen != NULL) // find matching
      //   {
      //     if(lisatom(opparen, PARENCL)) parendep++;
      //     if(lisatom(opparen, PARENOP)) parendep--;
      //     assert(parandep >= 0);
      //     opparen = opparen->left;
      //   }

      //   link *outl = opparen;
      //   opparen = opparen->right->right; // move in

      //   // detach
      //   opparen->left = NULL;
      //   curl->left->left->right = NULL;

      //   // evaluate
      //   token *abstype = ll2tokl(opparen); // convert to token list
      //                                      // evaluate
      //   ctype *ct = parsedecl(abstype, 1); // parse only declaration

      //   // new expression
      //   expr *newe = malloc(sizeof(expr));
      //   newe->type = UNAR_E;
      //   newe->optype = CAST_O;

      //   newe->args = malloc(sizeof(expr));
      //   newe->args[0] = curl->cont.exp;
      //   newe->ct = ct; // tell it what to cast to
      //   curl->cont.exp = newe;

      //   // absorb cast
      //   attach(outl, curl);
      // }

      else if(leistype(curl, UNAR_E) && lisatom(curl->left, SIZEOF)) // sizeof unary-expression
      {
        // TODO (this is a compile-time expression, must be evaluated now or soon)
        modified = 1;
      }

      else if(leistype(curl, TYPENAME) && lisatom(curl->left, SIZEOF)) // sizeof(type-name)
      {
        // TODO (see above)
        modified = 1;
      }

      else
      {
        if(curl->left == NULL) break; // break preemptively
        curl = curl->left; // move on
      }
    } while(1);

    // because we broke preemptively curl is not NULL
    leftend(curl);
    
    // unary -> cast
    link *temp = curl;
    while(temp != NULL)
    {
      if(leistype(temp, UNAR_E))
      {
        etypeadd(temp->cont.exp,CAST_E);
      }
      temp = temp->right;
    }

    rightend(curl);

    // cast expressions
    do
    {
      if(leistype(curl, CAST_E) && leistype(curl->left, TYPENAME)) // cast
      {
        expr *newe = calloc(1,sizeof(expr));
        etypeadd(newe,CAST_E);
        newe->optype = CAST_O;
        newe->args = malloc(sizeof(expr)*2); // cast, expr
        newe->args[0] = curl->left->cont.exp; // cast
        newe->args[1] = curl->cont.exp; // expr

        curl->cont.exp = newe;

        attach(curl->left->left, curl);

        modified = 1;
      }

      else
      {
        if(curl->left == NULL) break;
        curl = curl->left;
      }
    } while(1);

  } while(modified);


  leftend(curl);
  return curl;
}

// the following functions are extremely repetitive and therefore poorly implemented

// multiplicative expression
link *parsemultexpr(link *chain)
{
  // parse cast expression
  chain = parsecastunaryexpr(chain);

  // cast -> mult
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, CAST_E))
    {
      etypeadd(temp->cont.exp,MULT_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, MULT_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, STAR) && leistype(curl->right->right, CAST_E)) // mult-expr * cast-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,MULT_E);
      newe->optype = MULT_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, DIV) && leistype(curl->right->right, CAST_E)) // mult-expr / cast-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,MULT_E);
      newe->optype = DIV_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, MOD) && leistype(curl->right->right, CAST_E)) // mult-expr % cast-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,MULT_E);
      newe->optype = MOD_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;
}

// additive expression
link *parseaddexpr(link *chain)
{
  // parse mult expr
  chain = parsemultexpr(chain);

  // mult -> add
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, MULT_E))
    {
      etypeadd(temp->cont.exp,ADD_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, ADD_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, PLUS) && leistype(curl->right->right, MULT_E)) // add-expr + mult-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,ADD_E);
      newe->optype = ADD_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, MIN) && leistype(curl->right->right, MULT_E)) // add-expr - mult-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,ADD_E);
      newe->optype = SUB_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;
}

link *parseshiftexpr(link *chain)
{
  // parse add expr
  chain = parseaddexpr(chain);

  // add -> shift
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, ADD_E))
    {
      etypeadd(temp->cont.exp,SHIFT_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, SHIFT_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, SHL) && leistype(curl->right->right, ADD_E)) // shift-expr << add-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,SHIFT_E);
      newe->optype = SHL_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, SHR) && leistype(curl->right->right, ADD_E)) // shift-expr >> add-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,SHIFT_E
      newe->optype = SHR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;
}

link *parserelexpr(link *chain)
{
  // parse shift
  chain = parseshiftexpr(chain);

  // shift -> rel
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, SHIFT_E))
    {
      etypeadd(temp->cont.exp,RELAT_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, RELAT_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, LESS) && leistype(curl->right->right, SHIFT_E)) // rel-expr < shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = LT_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, GREAT) && leistype(curl->right->right, SHIFT_E)) // rel-expr > shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = GT_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, LEQ) && leistype(curl->right->right, SHIFT_E)) // rel-expr <= shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = LEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, GEQ) && leistype(curl->right->right, SHIFT_E)) // rel-expr >= shift-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,RELAT_E);
      newe->optype = GEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parseeqexpr(link *chain)
{
  // parse rel expression
  chain = parserelexpr(chain);

  // rel -> eq
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, RELAT_E))
    {
      etypeadd(temp->cont.exp,EQUAL_E);
    }
    temp = temp->right;
  }
  
  while(1)
  {
    if(!leistype(curl, EQUAL_O))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, EQEQ) && leistype(curl->right->right, RELAT_E)) // eq-expr == rel-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,EQUAL_E);
      newe->optype = EQEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else if(lisatom(curl->right, NOTEQ) && leistype(curl->right->right, RELAT_E)) // eq-epr != rel-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,EQUAL_E);
      newe->optype = NEQ_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parseandexpr(link *chain)
{
  // parse eq expression
  chain = parseeqexpr(chain);

  // eq -> and
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, EQUAL_E))
    {
      etypeadd(temp->cont.exp,AND_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, AND_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, BITAND) && leistype(curl->right->right, EQUAL_E)) // and-expr & eq-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,AND_E);
      newe->optype = BAND_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parsexorexpr(link *chain)
{
  // parse and expression
  chain = parseandexpr(chain);

  // and -> xor
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, AND_E))
    {
      etypeadd(temp->cont.exp,XOR_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, XOR_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, XOR) && leistype(curl->right->right, AND_E)) // xor-expr ^ and-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,XOR_E);
      newe->optype = XOR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parseorexpr(link *chain)
{
  // parse xor expression
  chain = parsexorexpr(chain);

  // xor -> or
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, XOR_E))
    {
      etypeadd(temp->cont.exp,OR_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, OR_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, OR) && leistype(curl->right->right, XOR_E)) // or-expr | xor-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,OR_E);
      newe->optype = BOR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

link *parselandexpr(link *chain)
{
  // parse or expression
  chain = parseorexpr(chain);

  // or -> land
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, OR_E))
    {
      etypeadd(temp->cont.exp,LAND_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, LAND_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, LOGAND) && leistype(curl->right->right, OR_E)) // land-expr && or-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,LAND_E);
      newe->optype = LAND_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;


}

link *parselorexpr(link *chain)
{
  // parse land expression
  chain = parselandexpr(chain);

  // land -> lor
  link *curl = chain;
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, LAND_E))
    {
      etypeadd(temp->cont.exp,LOR_E);
    }
    temp = temp->right;
  }

  while(1)
  {
    if(!leistype(curl, LOR_E))
    {
      curl = curl->right;
      continue;
    }

    if(lisatom(curl->right, LOGOR) && leistype(curl->right->right, LAND_E)) // lor-expr || land-expr
    {
      expr *newe = calloc(1,sizeof(expr));
      etypeadd(newe,LOR_E);
      newe->optype = LOR_O;
      newe->args  =malloc(sizeof(expr)*2);
      newe->args[0] = curl->cont.exp;
      newe->args[1] = curl->right->right->cont.exp;

      curl->cont.exp = newe;

      attach(curl, curl->right->right->right);
    }

    else
    {
      if(curl->right == NULL) break;
      curl = curl->right;
    }
  }

  leftend(curl);
  return curl;

}

// TODO constant expressions

link *parsecondexpr(link *chain)
{
  chain = parselorexpr(chain);

  link *curl = chain;
  
  // promote
  link *temp = curl;
  while(temp != NULL)
  {
    if(leistype(temp, LOR_E))
    {
      etypeadd(temp->cont.exp, COND_E);
    }
    temp = temp->right;
  }

  rightend();
  while(1)
  {
    // lor-expr ? expr : cond-expr
    if(leistype(curl, COND_E) && lisatom(curl->left, COLON))
    {
      // find matching question for colon
      link *colon = curl->left;
      link *quest = findmatch(colon, LEFT, COLON, QUESTION);

      // make sure it's proper form
      assert(leistype(quest->left, LOR_E));

      // detach middle expression & parse
      colon->left->right = NULL;
      quest->right->left = NULL;
      link *l = parseexpr(quest->right);

      // create new
      expr *newe = calloc(1, sizeof(expr));
      etypeadd(newe, COND_E);
      newe->optype = TERN_O;
      newe->args = malloc(sizeof(expr)*3);
      newe->args[0] = quest->left->cont.exp;
      newe->args[1] = l->cont.exp;
      newe->args[2] = curl->cont.exp;
      curl->cont.exp = newe;

      // attach
      attach(quest->left->left, curl);
    }

    else
    {
      if(curl->left == NULL) break;
      curl = curl->left;
    }
  }

  leftend(curl);
  return curl;
}


int main()
{
  // TODO fix the literary hierarchy, it's still broken
  // should be doable with 1 run-through
  // TODO constantly print to stderr what token is being read, what line number, etc. so that when asserts fail it's immediately clear where it happened

  assert(sizeof(float) == 4); // there is no int32_t analog for floats

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

  char *quot = malloc(i);
  char *esc = malloc(i);

  splice(src); // delete backslash + newline combinations

  mark_esc(src, esc); // mark backslash escape sequences
  mark_quot(src, esc, quot); // mark single and double quoted regions

  rem_comments(src, esc, quot); // replace multiline comments with single space

  // mark AGAIN now that comments are gone (stray quotes will break things
  mark_esc(src, esc); // mark backslash escape sequences
  mark_quot(src, esc, quot); // mark single and double quoted regions

  // stray_backslash(src, esc, quot); // check for stray backslashes, throw a tantrum if so
  check_stray(src, esc, quot, "#$@\\`"); // check for stray characters, throw a tantrum if so


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


  // tokenize
  // alloc(token, trans_unit, tsize, tcount);

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
    puttok(*(token *)last(trans_unit));

  }
  while(((token *)last(trans_unit))->gen.type != NOTOK);

  link *chain = tokl2ll((token *)trans_unit->cont);
  puts("-------------------");
  putll(chain);
  puts("-------------------");
  chain = parsepostexpr(chain);
  puts("--");
  putll(chain);
  

  // puts("------------------");
  // while(parsedecl((token *)trans_unit->cont)); // parse until NOTOK

  // tcount--; // exclude NOTOK

  // turn tokens into linked list (translation unit)
  // link *trans_unit = malloc(sizeof(link) * tcount);
  // link *prevl = NULL; // previous link
  // for(i = 0; i < tcount; i++)
  // {
  //     tok_chain[i].left = prevl;
  //     tok_chain[i].right = NULL;
  //     if(prevl != NULL)
  //     {
  //       prevl->right = tok_chain+i;
  //     }
  //     prevl = tok_chain+i;

  //     tok_chain[i].type = TOK_L;
  //     tok_chain[i].cont.tok = toks+i;
  // }





  // link ** decls = calloc(tcount, sizeof(link *) ); // declarations, a list of linked lists
  // int numdecl = 0;
  // int bracedepth = 0; // used to tell when a declaration is ending
  // link *cur_l = tok_chain;
  // do
  // {
  //   if(!decls[numdecl]) // start of new declaration
  //   {
  //     decls[numdecl] = cur_l;
  //     // puttok(*cur_l->cont.tok);
  //     cur_l->left = NULL; // start of linked list for declaration
  //   }

  //   link *right = cur_l->right; // may be overwritten soon if declaration ends

  //   token *cur_tok = cur_l->cont.tok;

  //   if(isatom(cur_tok, BRACEOP))
  //     bracedepth++;
  //   if(isatom(cur_tok, BRACECL))
  //   {
  //     bracedepth--;
  //     if(bracedepth == 0) // must be end of statement
  //     {
  //       if(right && isatom(right->cont.tok, SEMICOLON)) // presume end of declaration
  //       {
  //         cur_l = right->right;
  //         right->right = NULL;

  //         numdecl++;
  //         continue;
  //       }
  //       else // presume end of function
  //       {
  //         cur_l->right = NULL;
  //         cur_l = right;

  //         numdecl++;
  //         continue;
  //       }
  //     }
  //   }

  //   if(isatom(cur_l->cont.tok, SEMICOLON) && bracedepth == 0) // end of declaration
  //   {
  //     numdecl++;
  //     cur_l->right = NULL;
  //   }

  //   cur_l = right;
  // } while(cur_l);

  // assert(decls[numdecl] == NULL); // otherwise we have an unfinished declaration

  // for(i = 0; i < numdecl; i++)
  // {
  //   puttok(*decls[i]->cont.tok);
  // }






  // separate top-level declarations
  // link *trans_unit = malloc(;
  

  // print escape and quote markers
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
