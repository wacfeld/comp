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
typedef set stack; // idem

// lists tend to be used when we are reading data and don't know the length yet. they are clumsy to use because of the variable type size, so after everything is read, it should be converted to a more direct data type (e.g. makelist(sizeof(int)) -> int *)

stack *makestack(int size);
void peek(stack *s, void *m);
void pop(stack *s, void *m);
void push(stack *s, void *m);

set *makeset(int size);
int setins(set *s, void * m);
int setrem(set *s, void *m);
int inset(set *s, void * m);

// set *copyset(set *s);

int issub(set *sub, set *sup);
int seteq(set *s1, set *s2);

void append(list *l, void *m);
void *get(list *l, int i);
void listget(list *l, int  i, void *m);


void *last(list *l);
list *makelist(int size);
void reverse(list *l);
void clear(list *l);
void freeset(set *s);

void *first(list *l);
list *rest(list *l);

int isempty(list *l);

void rem_front(list *l); // like a dequeue function, but doesn't return anything
void shift_front(list *l); // non-destructive, just moves pointer forward and decrements n

int intinset(set *s, int x);
void intsetins(set *s, int x);

int listlen(list *l);

#endif
