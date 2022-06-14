#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define add1(x) x##1
// shortcut for strapp
#define appmac(dest, src) {dest = strapp(dest, &dest##_len, src);}

// count arguments
#define _GET_NTH_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, N, ...) N
#define COUNT_ARGS(...) _GET_NTH_ARG("ignored", ##__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// shortcut for multiapp using COUNT_ARGS
#define mapmac(dest, ...) {dest = multiapp(dest, &dest##_len, COUNT_ARGS(__VA_ARGS__), ##__VA_ARGS__)}
int main()
{
mapmac(assem, 1);
  return 0;
}

