typedef struct
{
  int *cont;
  int n;
  int size;
} set;

set *makeset(int size);
int setins(set *s, int m);
int inset(set *s, int m);


