/////////////////////////////////////////////////////////////////////////////
// INTEGRITY STATEMENT (v3)
//
// By signing your name and ID below you are stating that you have agreed
// to the online academic integrity statement:
// https://student.cs.uwaterloo.ca/~cs136/current/assignments/integrity.shtml
/////////////////////////////////////////////////////////////////////////////
// I received help from and/or collaborated with:

// nobody
//
// Name: Daniel Yu
// login ID: dd4yu
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
typedef int bool;
#include <stdio.h>

// struct rtnl {
//   int num;
//   int den;
// };
// // requires: den != 0


// // rtnl_simplify(r) turns r into simplified form
// void rtnl_simplify(struct rtnl *r) {
//   assert(r->den != 0);
//   if(r->den < 0) {
//     r->den = -r->den;
//     r->num = -r->num;
//   }
//   int sign = (r->num < 0) ? -1 : 1;
//   r->num = (r->num < 0) ? -r->num : r->num;
  
//   int a = r->num;
//   int b = r->den;
//   int gcd;

//   // apply euclidean algorithm
//   while(b)
//   {
//     a = a % b;
//     int temp = a;
//     a = b;
//     b = temp;
//   }
  
//   gcd = a;

//   r->num /= gcd;
//   r->den /= gcd;
//   r->num *= sign;
// }


// // rtnl_init(num, den) returns num/den in simplified form
// struct rtnl rtnl_init(int num, int den) {
//   struct rtnl r = {num,den};
//   rtnl_simplify(&r);
//   return r;
// }

// // rtnl_add(r, s) returns r+s
// struct rtnl rtnl_add(const struct rtnl *r, const struct rtnl *s) {
//   struct rtnl t = rtnl_init(r->num*s->den + s->num*r->den, r->den*s->den);
//   return t;
// }

// // rtnl_sub(r, s) returns r - s
// struct rtnl rtnl_sub(const struct rtnl *r, const struct rtnl *s) {
//   struct rtnl t = rtnl_init(-s->num, s->den);
//   return rtnl_add(r, &t);
// }



// // rtnl_mul(r, s) returns r*s
// struct rtnl rtnl_mul(const struct rtnl *r, const struct rtnl *s) {
//   return rtnl_init(r->num * s->num, r->den * s->den);
// }



// // rtnl_equal(r, s) returns whether r and s are equivalent rational numbers
// bool rtnl_equal(const struct rtnl *r, const struct rtnl *s) {
//   struct rtnl a = rtnl_init(r->num, r->den);
//   struct rtnl b = rtnl_init(s->num, s->den);
//   return (a.num == b.num) && (a.den == b.den);
// }


// // point_3d_dist_squared(p, q) returns the euclidean distance between p and q
// struct rtnl point_3d_dist_squared(const struct point_3d *p, const struct point_3d *q) {
//   struct rtnl d1 = rtnl_sub(p->x, q->x);
//   struct rtnl d2 = rtnl_sub(p->y, q->y);
//   struct rtnl d3 = rtnl_sub(p->z, q->z);

//   struct rtnl s1 = rtnl_mul(d1, d1);
//   struct rtnl s2 = rtnl_mul(d2, d2);
//   struct rtnl s3 = rtnl_mul(d3, d3);

//   return rtnl_add(rtnl_add(s1, s2), s3);
// }

int lastc = 0;

int mygetchar() {
  if(lastc) {
    int temp = lastc;
    lastc = 0;
    return temp;
  }
  else {
    return getchar();
  }
}

int ungetchar(int c) {
  lastc = c;
}

int main(void) {
  int c;
  
  while((c = mygetchar()) != EOF) {
    if(c == '/') {
      c = mygetchar();
      if(c == '/') {
        while((c = mygetchar()) != '\n') {
          if(c == EOF) {
            return 0;
          }
        }
        putchar('\n');
      }
      else if(c == '*') {
        while(1) {
          while((c = mygetchar()) != '*') {
            if(c == EOF) {
              return 0;
            }
          }

          if((c = mygetchar()) == '/') {
            break;
          } else {
            ungetchar(c);
          }

          if(c == EOF) {
            return 0;
          }
        }
      }
      else {
        putchar('/');
      }
    }

    else {
      putchar(c);
    }

    if(c == '"') {
      while((c = mygetchar()) != EOF) {
        putchar(c);
        if(c == '\\') {
          if((c = mygetchar()) == EOF) {
            return 0;
          }
          putchar(c);
        }
        else if(c == '"') {
          putchar(c);
          break;
        }
      }
      if(c == EOF) {
        return 0;
      }
    }
    
    if(c == EOF) {
      break;
    }
  }
}
