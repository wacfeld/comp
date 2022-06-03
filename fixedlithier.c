// NOT ACTUALLY FIXED, DOESN'T ACCOUNT FOR '"' for example

#include <assert.h>
typedef int bool;
#include <stdio.h>
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
