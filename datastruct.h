#ifndef DATASTRUCT
#define DATASTRUCT
typedef struct
{
  char *cont; // content
  int max; // maximum length (currently allocated)
  int n; // current length
  int size; // size of data type
} set;

typedef set list; // same data structure, different functions

set *makeset(int size);
int setins(set *s, void * m);
int setrem(set *s, void *m);
int inset(set *s, void * m);

void append(list *l, void *m);
void *get(list *l, int i);

void *last(list *l);
list *makelist(int size);
void reverse(list *l);
void clear(list *l);

void *first(list *l);
list *rest(list *l);
#endif
