#include "datastruct.h"
#include "defs.h"

typedef struct
{
  int *cont;
  int size;
  int n;
} set;

typedef set list; // same data structure, different functions


set *makeset(int size)
{
  set *s = malloc(sizeof(set));
  s->cont = malloc(sizeof(int) * size);
  s->size = size;
  s->n = 0;
}

int setins(set *s, int m) // set insert, returns 1 if already in set
{
  for(int i = 0; i < s->n; i++)
  {
    if(s->cont[i] == m) // already in set
    {
      return 1;
    }
  }

  // insert into set and resize if necessary
  resize(s->cont, s->size, s->n);
  s->cont[s->n] = m;
  s->n += 1;
  return 0;
}

int inset(set *s, int m)
{
  for(int i = 0; i < s->n; i++)
  {
    if(s->cont[i] == m)
    {
      return 1;
    }
  }
  return 0;
}


list *makelist(int size)
{
  return makeset(size);
}

void append(list *l, int m) // list append
{
  resize(l->cont, l->size, l->n);
  l->cont[l->n] = m;
  l->n += 1;
}

int get(list *l; int i) // list get
{
  return l.cont[i];
}
