#include "defs.h"

int liscomma(link *l)
{
  return lisatom(l, COMMA);
}


int isasgnop(link *l)
{
  return lisatom(l, EQ)
    || lisatom(l, DIVEQ)
    || lisatom(l, MODEQ)
    || lisatom(l, PLUSEQ)
    || lisatom(l, MINEQ)
    || lisatom(l, SHLEQ)
    || lisatom(l, SHREQ)
    || lisatom(l, ANDEQ)
    || lisatom(l, XOREQ)
    || lisatom(l, OREQ);
}

int isquest(link *l)
{
  return lisatom(l, QUESTION);
}
