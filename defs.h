#ifndef DEFS
#define DEFS

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
#include <stdarg.h>
// #include <stddef.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>

#include "datastruct.h"

#define ff(s) {puts("================");puts(s);puts("================");}

#define putd(x) printf(#x ": %d\n", x)
#define putp(x) printf(#x ": %p\n", x)
#define putf(x) printf(#x ": %f\n", x)

#define streq(a, b) (!strcmp((a), (b)))


// when debug is on all here() calls activate
#define DEBUG 0
#define here() {if(DEBUG) {printf("%d %s\n", __LINE__, __func__);}}
#define nline() puts("")

// use realloc when necessary to expand a dynamically allocated array
#define resize(p, s, c) if(c >= s) {s *= 2; p = realloc(p, s*sizeof(p[0]));}

#define alloc(type, str, size, c) int size = 10; int c = 0; type *str = malloc(size*sizeof(type));
// #define alloctok(str, size, c) int size = 10; int c = 0; tok *str = malloc(size*sizeof(tok));
#define read(str, size, c, src, i) str[c++] = src[i++]; resize(str, c, size);

#define max(a,b) (((a) < (b)) ? (b) : (a))

#define CHAR_MAX 255
#define CHAR_SIZE 1
#define FLOAT_SIZE 4
#define INT_SIZE 4
#define PTR_SIZE 4
// doubles and long doubles are the same size as floats
// all int sizes are the same as int
// this complies with the standard

#define INT_MAX 2147483647
// the largest a signed int can be (inclusive)

enum tok_type {NOTOK=0, ERRTOK, KEYWORD, IDENT, STRLIT, CHAR, UNCERTAIN, INTEGER, FLOATING, ATOM};

enum atom_type {/*FCALL, ARRIND,*/ ARROW, DOT, LOGNOT, BITNOT, INC, DEC, UNPLUS, UNMIN, DEREF, CAST, SIZEOF, TIMES, DIV, MOD, BINPLUS, BINMIN, SHL, SHR, LESS, LEQ, GREAT, GEQ, EQEQ, NOTEQ, BITAND, BITXOR, BITOR, LOGAND, LOGOR, TERNARY, EQ, PLUSEQ, MINEQ, TIMESEQ, DIVEQ, MODEQ, ANDEQ, XOREQ, OREQ, SHLEQ, SHREQ, COMMA, PLUS, MIN, STAR, COLON, QUESTION, SEMICOLON, PARENOP, PARENCL, BRACEOP, BRACECL, BRACKOP, BRACKCL};

typedef enum tok_type tok_type;
typedef enum int_len int_len;

typedef u_int32_t dword; // double word, 32 bits, which can hold all primitive types in this implementation

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
    int len; // we need len because it might have user-added \0's in the string
    // len includes the automatically added terminating null
  } strlit;

  struct
  {
    tok_type type;
    char cont;
  } character; // char is reserved

  struct
  {
    tok_type type;
    dword cont;
    int islong;
    int isunsigned;
    int isdecimal;
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

} token;

// human readable, for debugging purposes only
char *hrtok[100] = {
[NOTOK]="NOTOK", [ERRTOK]="ERRTOK", [KEYWORD]="KEYWORD", [IDENT]="IDENT", [STRLIT]="STRLIT", [CHAR]="CHAR", [UNCERTAIN]="UNCERTAIN", [INTEGER]="INTEGER", [FLOATING]="FLOATING", [SEMICOLON]="SEMICOLON", [PARENOP]="PARENOP", [PARENCL]="PARENCL", [BRACEOP]="BRACEOP", [BRACECL]="BRACECL", [BRACKOP]="BRACKOP", [BRACKCL]="BRACKCL", [ATOM]="ATOM"};

char *hrat[100] = {
  // [FCALL]="FCALL",
// [ARRIND]="ARRIND",
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

enum keyword { K_NONE = 0, K_AUTO, K_BREAK, K_CASE, K_CHAR, K_CONST, K_CONTINUE, K_DEFAULT, K_DO, K_DOUBLE, K_ELSE, K_ENUM, K_EXTERN, K_FLOAT, K_FOR, K_GOTO, K_IF, K_INT, K_LONG, K_REGISTER, K_RETURN, K_SHORT, K_SIGNED, K_STATIC, K_STRUCT, K_SWITCH, K_TYPEDEF, K_UNION, K_UNSIGNED, K_VOID, K_VOLATILE, K_WHILE}; // sizeof not here because it's an operator
char *keywords[] =
{
  [K_NONE]="K_NONE",
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
  [K_WHILE]="while"
};

struct expr;
typedef struct expr expr;

// initializer, can be just an expression, or a list of initializers!
struct init
{
  int islist;
  struct init **lst;
  int len;

  expr *e;
};

typedef struct decl decl;

typedef union
{
  struct
  {
    int type;
  } gen;

  struct
  {
    int type;
    int isconst;
    int isvolat;
  } ptr;

  struct
  {
    int type;
    int len;
  } arr;

  struct
  {
    int type;
    // list *params; // list of decls
    decl *params; // list of decls
    int np; // number of params
    // TODO parameters
  } func;

  struct
  {
    int type;
    char *name;
  } ident; // we pretend the identifier is a typemod for convenience (e.x. this makes gettypemods() a little cleaner)

  struct
  {
    int type;
    int isconst;
    int isvolat;
    int dt;
  } dat;
  // note thate this means that checking equality of types will require checking for possible identifiers

} typemod; // type modifier


// typedef struct
// {
//   // int dattype; // e.g. INT_T, SHORT_T, VOID_T, etc.

//   // int storespec; // NOSPEC, EXTERN_S, STATIC_S

//   // int isconst;
//   // int isvolat;

//   typemod *tms; // end indicated by TM_NONE
//   // int tmlen;

// } ctype; // type, ALWAYS ABSTRACT.

typedef typemod *ctype;

enum storespec {NOSPEC = 0, EXTERN_S, STATIC_S, REGISTER_S, AUTO_S};
int specmap[] = {
  [K_EXTERN]=EXTERN_S,
  [K_STATIC]=STATIC_S,
  [K_REGISTER]=REGISTER_S,
  [K_AUTO]=AUTO_S,
};

// enum storeloc {LOCAL, GLOBAL}; // stored on stack or in data/bss
struct location
{
  int global; // whether it's global (top level, bss/data) or local (stack)
  char *globloc; // nasm label
  int locloc; // offset from ebp
};

// one or more statements, in the form of a token array
struct stat
{
  token *toks;
  int lo;
  int hi; // inlcusive
};

typedef struct decl
{
  // set *typespecs;
  // int dattype;
  // set *typequals;
  // int storespec;

  char *ident;
  struct location locat;

  int storespec;
  ctype ct;


  // int isconst;
  // int isvolat;

  // list *typemods;
  // int lval; // TODO

  struct init *init; // e.g. {1,2, {3,4,5}}
  struct stat *fundef; // TODO

} decl;

// operator types, "subtypes" of expressions
// we could just borrow the duplicate atom names from above, but oh well
enum optype
{
  // primary expressions
  IDENT_O,
  STRING_O,
  // constant expressions
  INT_O,
  FLOAT_O,
  CHAR_O,
  ENUM_O,
  // PAREN_O, // (expression), which is a primary expression
  // ^^^ expressions that return their data as-is
  
  // postfix expressions
  // ARR_O, // always replaced with the equivalent pointer + int expression
  FUN_O,
  STRUCT_O, // .
  PSTRUCT_O, // ->
  POSTINC_O,
  POSTDEC_O,
  
  // unary expressions
  SIZEOF_O,
  PREINC_O,
  PREDEC_O,
  // it's more convenient to just use expr.type for parsing, and just use expr.optype for evaluation. therefore we must differentiate between PRE/POST INC/DEC operators

  // unary operators
  ADDR_O, // &
  POINT_O, // *
  UPLUS_O,
  UMIN_O,
  BNOT_O,
  LNOT_O,

  // cast expressions
  CAST_O, // likely unnecessary
  
  // multiplicative expressions
  MULT_O,
  DIV_O,
  MOD_O,

  // additive expressions
  ADD_O,
  SUB_O,
  
  // shift expressions
  SHL_O,
  SHR_O,

  // relational expressions
  LT_O,
  GT_O,
  LEQ_O,
  GEQ_O,

  // equality expressions
  EQEQ_O,
  NEQ_O,

  // AND expressions
  BAND_O,
  
  // exclusive OR expressions
  XOR_O,

  // inclusive OR expressions
  BOR_O,

  // logical AND
  LAND_O,

  // logical OR
  LOR_O,
  
  // conditional expressions
  TERN_O,

  // assignment expressions
  EQ_O,
  TIMESEQ_O,
  DIVEQ_O,
  MODEQ_O,
  PLUSEQ_O,
  MINEQ_O,
  SHLEQ_O,
  SHREQ_O,
  ANDEQ_O,
  XOREQ_O,
  OREQ_O,

  COMMA_O,
  // ARR2PTR_O,
  // LVAL2DAT_O,
  // FUN2PTR_O,
};

char *hropt[1000] =
{
  // [FUN2PTR_O]="FUN2PTR_O",
  [IDENT_O]="IDENT_O",
  // [ARR2PTR_O]="ARR2PTR_O",
  // [LVAL2DAT_O]="LVAL2DAT_O",
  [STRING_O]="STRING_O",
  [INT_O]="INT_O",
  [FLOAT_O]="FLOAT_O",
  [CHAR_O]="CHAR_O",
  [ENUM_O]="ENUM_O",
  // [PAREN_O]="PAREN_O",
  // [ARR_O]="ARR_O",
  [FUN_O]="FUN_O",
  [STRUCT_O]="STRUCT_O",
  [PSTRUCT_O]="PSTRUCT_O",
  [POSTINC_O]="POSTINC_O",
  [POSTDEC_O]="POSTDEC_O",
  [SIZEOF_O]="SIZEOF_O",
  [PREINC_O]="PREINC_O",
  [PREDEC_O]="PREDEC_O",
  [ADDR_O]="ADDR_O",
  [POINT_O]="POINT_O",
  [UPLUS_O]="UPLUS_O",
  [UMIN_O]="UMIN_O",
  [BNOT_O]="BNOT_O",
  [LNOT_O]="LNOT_O",
  [CAST_O]="CAST_O",
  [MULT_O]="MULT_O",
  [DIV_O]="DIV_O",
  [MOD_O]="MOD_O",
  [ADD_O]="ADD_O",
  [SUB_O]="SUB_O",
  [SHL_O]="SHL_O",
  [SHR_O]="SHR_O",
  [LT_O]="LT_O",
  [GT_O]="GT_O",
  [LEQ_O]="LEQ_O",
  [GEQ_O]="GEQ_O",
  [EQEQ_O]="EQEQ_O",
  [NEQ_O]="NEQ_O",
  [BAND_O]="BAND_O",
  [XOR_O]="XOR_O",
  [BOR_O]="BOR_O",
  [LAND_O]="LAND_O",
  [LOR_O]="LOR_O",
  [TERN_O]="TERN_O",
  [COMMA_O]="COMMA_O",
  [EQ_O]="EQ_O",
  [TIMESEQ_O]="TIMESEQ_O",
  [DIVEQ_O]="DIVEQ_O",
  [MODEQ_O]="MODEQ_O",
  [PLUSEQ_O]="PLUSEQ_O",
  [MINEQ_O]="MINEQ_O",
  [SHLEQ_O]="SHLEQ_O",
  [SHREQ_O]="SHREQ_O",
  [ANDEQ_O]="ANDEQ_O",
  [XOREQ_O]="XOREQ_O",
  [OREQ_O]="OREQ_O",
};

typedef enum expr_type
{
  EXPR,
  PRIM_E,
  POST_E,
  UNAR_E,
  CAST_E,
  MULT_E,
  ADD_E,
  SHIFT_E,
  RELAT_E,
  EQUAL_E,
  AND_E,
  XOR_E,
  OR_E,
  LAND_E,
  LOR_E,
  COND_E,
  ASGN_E,
  COMMA_E,
  CONST_E,
  TYPENAME, // special case: for casts
  ARGLIST, // special case: for function calls
  DECAY, // decay to pointer or nullptr
} expr_type;

char *hr_expr[100] = 
{
  [EXPR]="EXPR",
  [PRIM_E]="PRIM_E",
  [DECAY]="DECAY",
  [POST_E]="POST_E",
  [UNAR_E]="UNAR_E",
  [CAST_E]="CAST_E",
  [MULT_E]="MULT_E",
  [ADD_E]="ADD_E",
  [SHIFT_E]="SHIFT_E",
  [RELAT_E]="RELAT_E",
  [EQUAL_E]="EQUAL_E",
  [AND_E]="AND_E",
  [XOR_E]="XOR_E",
  [OR_E]="OR_E",
  [LAND_E]="LAND_E",
  [LOR_E]="LOR_E",
  [COND_E]="COND_E",
  [ASGN_E]="ASGN_E",
  [COMMA_E]="COMMA_E",
  [CONST_E]="CONST_E",
  [TYPENAME]="TYPENAME",
  [ARGLIST]="ARGLIST",
};

// HERE
struct expr
{
  // expr_type type;
  // the bottom-up evaluation apparatus required that an expr could be multiple types. this is no longer the case but it remains a set for consistency
  // set *type; // multiple types at once are possible

  int type;
  
  int optype;
  struct expr **args;
  int numargs; // sometimes necessary, eg. function arguments

  // token *tok; // probably only for constants // temporary solution, may need more general/specific way to encode the relevant data

  dword dat; // 32 bits of data, for compile-time constants
  char *strlit; // string literals! only used occasionally
  decl * dcl; // for objects, functions, etc. which can appear from primary expressions

  // TODO ^^

  int lval;
  ctype ct; // expressions get parsed top to bottom, and then get assembled bottom to top. we get the ctypes of primary expressions and then build up, so that we always know what type everything should be

};
// typedef struct expr expr;


enum link_type {EXPR_L, TOK_L};

// for making linked lists of tokens
typedef struct link
{
  struct link *left;
  struct link *right;

  enum link_type type;
  union
  {
    token *tok;
    expr *exp;
  } cont;
} link;

// linked lists seem like a good way to represent expressions


// typedef struct
// {
//   int *typespecs; // array of ints (mix of enum keywords and user-defined types, but all ints)
//   int *typequals; // similar
//   int *storespecs; // similar

//   // TODO the rest

// } declaration;

// typemod type
// enum tmt {TM_PTR, TM_ARR, TM_FUNC, TM_IDENT, TM_NONE, TM_DAT};
enum tmt {TM_PTR, TM_ARR, TM_FUNC, TM_IDENT, TM_DAT};
// TM_NONE is not needed as TM_DAT will terminate the list

enum stattype {LAB_S, EXPR_S, COMP_S, SEL_S, ITER_S, JUMP_S};


int canbeunary[] = {BITAND, PLUS, MIN, STAR}; // atoms that represent both unary and binary operators
int cbulen = sizeof(canbeunary)/sizeof(int);


enum dattype {NONE_T = 0, VOID_T, CHAR_T, UCHAR_T, INT_T, UINT_T, SINT_T, LINT_T, USINT_T, ULINT_T, FLOAT_T, DUB_T, LDUB_T};

char *hrdt[] =
{
  [VOID_T]="VOID_T",
  [CHAR_T]="CHAR_T",
  [UCHAR_T]="UCHAR_T",
  [INT_T]="INT_T",
  [UINT_T]="UINT_T",
  [SINT_T]="SINT_T",
  [LINT_T]="LINT_T",
  [USINT_T]="USINT_T",
  [ULINT_T]="ULINT_T",
  [FLOAT_T]="FLOAT_T",
  [DUB_T]="DUB_T",
  [LDUB_T]="LDUB_T",
};

// superset: left type quals must be superset of right type quals (for assignment)
// nocare: quals can be whatever (for function params)
// strict: quals must match perfectly (for multiple declarations)
enum qualmode {QM_SUPERSET, QM_NOCARE, QM_STRICT};

// esi, edi, ebp, esp are not necessary for this enum
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


link *nexttoplevel(link *start, int dir, int num, int *atoms);
expr *parseexpr(link *start);
expr *parseasgnexpr(link *start);
expr *parsecondexpr(link *start);
expr * parseltrbinexpr(link *start, int etype, int num, int *atoms, int *optypes, expr *(*down)(link *), expr *(*side)(link *));
expr *parselorexpr(link *start);
expr *parselandexpr(link *start);
expr *parseorexpr(link *start);
expr *parsexorexpr(link *start);
expr *parseandexpr(link *start);
expr *parseeqexpr(link *start);
expr *parserelexpr(link *start);
expr *parseshiftexpr(link *start);
expr *parseaddexpr(link *start);
expr *parsemultexpr(link *start);
expr *parsecastexpr(link *start);
expr *parseunaryexpr(link *start);
ctype parsetypename(link *start);
expr *parsepostexpr(link *start);
expr *parsearglist(link *start);
expr *parseprimexpr(link *start);

expr *makeexpr(int type, int optype, int numargs, ...);
ctype makedt(int dt);


decl *getdeclspecs(token *toks, int *i);

int tmis(typemod *tm, int type);
decl *searchscope(char *ident);

int validct(ctype ct);
int incomplete(ctype ct);
int ctisdt(ctype ct, int dt);
int iscompat(ctype ct1, ctype ct2, int qualmode);
int getctlen(ctype ct);

ctype unqual(ctype ct);

char *parsestat(struct stat *stat);

void pushdecl(decl *d);


char *stackalloc(int size)           ;
char *stackdealloc(int size);
char *imm2stack(decl *d, int imm)    ;
char *reg2stack(decl *d, char *reg)  ;
char *stack2reg(char *reg, decl *d)  ;


void pushnull();
void remtonull();

void helpputexpr(expr *e, int space);

expr *tokl2expr(token *toks, int lo, int hi);
link *tokl2ll(token *tokl, int len);

char *evalexpr(expr *e);

expr *tokl2expr(token *toks, int lo, int hi);

int asprintf(char **strp, const char *fmt, ...);

int isasgnop(enum optype o);

#endif
