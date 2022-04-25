#include "datastruct.h"
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


set *makeset(int size)
{
  set *s = malloc(sizeof(set));
  s->max = 10;
  s->cont = malloc(size * s->max);
  s->size = size;
  s->n = 0;
}

int setins(set *s, void* m) // set insert, returns 1 if already in set
{
  int size = s->size;
  for(int i = 0; i < s->n; i++)
  {
    if(!memcmp(s->cont + i*size, m, size)) // already in set
    {
      return 1;
    }
  }

  // insert into set and resize if necessary
  if(s->n >= s->max)
  {
    s->max *= 2;
    s->cont = realloc(s->cont, s->max * size);
  }

  memcpy(s->cont + s->n * size, m, size);
  s->n += 1;
  return 0;
}

int inset(set *s, void *m)
{
  int size = s->size;
  for(int i = 0; i < s->n; i++)
  {
    if(!memcmp(s->cont + i*size, m, size))
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

void append(list *l, void * m) // list append
{
  int size = l->size;
  if(l->n >= l->max)
  {
    l->max *= 2;
    l->cont = realloc(l->cont, l->max * size);
  }

  memcpy(l->cont + l->n * size, m, size);
  l->n += 1;
}

void *get(list *l, int i) // list get
{
  int size = l->size;
  void *m = malloc(size);
  memcpy(m, l->cont + i*size, size);
  return m;
}


void *last(list *l)
{
  return get(l, l->n-1);
}

void reverse(list *l)
{
  int size = l->size;
  char *newcont = malloc(size * l->max);
  for(int i = 0; i < l->n; i++)
  {
    memcpy(newcont+size*i, l->cont+size*(l->n-i-1), size);
  }
  free(l->cont);
  l->cont = newcont;
}

void clear(list *l) // don't free() anything, just make length 0 again
{
  l->n = 0;
}
