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

// use realloc when necessary to expand a dynamically allocated array
#define resize(p, c, s) if(c >= s) {s *= 2; p = realloc(p, s);}

#define allocstr(str, size, c) int size = 10; int c = 0; char *str = malloc(size);
#define read(str, size, c, src, i) str[c++] = src[i++]; resize(str, c, size);

#define CHAR_MAX 255
#define CHAR_SIZE 1
#define FLOAT_SIZE 4
#define INT_SIZE 4


enum tok_type {NOTOK, ERRTOK, KEYWORD, IDENT, STRLIT, CHAR, UNCERTAIN, INTEGER, FLOATING, ATOM};

enum atom_type {FCALL, ARRIND, ARROW, DOT, LOGNOT, BITNOT, INC, DEC, UNPLUS, UNMIN, DEREF, CAST, SIZEOF, TIMES, DIV, MOD, BINPLUS, BINMIN, SHL, SHR, LESS, LEQ, GREAT, GEQ, EQEQ, NOTEQ, BITAND, BITXOR, BITOR, LOGAND, LOGOR, TERNARY, TERNARYQUEST, EQ, PLUSEQ, MINEQ, TIMESEQ, DIVEQ, MODEQ, ANDEQ, XOREQ, OREQ, SHLEQ, SHREQ, COMMA, PLUS, MIN, STAR, COLON, QUESTION, SEMICOLON, PARENOP, PARENCL, BRACEOP, BRACECL, BRACKOP, BRACKCL};

typedef enum tok_type tok_type;
typedef enum int_len int_len;

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
  [K_WHILE]="while"
};
