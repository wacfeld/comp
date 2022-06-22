
void putchar(int x);
int getchar();
void exit();

void putstr(char *s)
{
  while(*s)
  {
    putchar(*s);
    s++;
  }
}

void puts(char *s)
{
  putstr(s);
  putchar('\n');
}

void putint(int x)
{
  if(x < 0)
  {
    putchar('-');
    x = -x;
  }


  char arr[100];
  int i = 0;
  
  do
  {
    arr[i++] = x%10 + '0';
    x = x / 10; 
  } while(x);
  
  for(i--; i >= 0; i--)
  {
    putchar(arr[i]);
  }
}

void putintarr(int *x, int len)
{
  int i;
  putstr("[");
  for(i = 0; i < len; i++)
  {
    putint(x[i]);
    if(i != len-1)
    {
      putstr(", ");
    }
  }
  putstr("]\n");
}

void fib(int n);
int factorial(int x);

int identifier = 5;

int main()
{
  
  puts("identifier (global):");
  putint(identifier);
  putchar('\n');

  {
    int identifier = 10;
    puts("identifier (local):");
    putint(identifier);
    putchar('\n');
    identifier++;
    putint(identifier);
    putchar('\n');
  }

  puts("identifier (global):");
  putint(identifier);
  putchar('\n');
  
  
  int len = 10;
  int x[10];
  x[6] = 11;
  x[2] = 7;
  x[4] = 6;
  x[0] = 5;
  x[5] = 5;
  x[1] = 4;
  x[9] = 7;
  x[8] = 0;
  x[3] = 1;
  x[7] = -2;

  
  putstr("bubble sort:\n");
  putintarr(x, len);
  
  int i, j;
  for(i = 0; i < 10; i++)
  {
    for(j = 0; j < 9; j++)
    {
      if(x[j] > x[j+1])
      {
        int temp = x[j];
        x[j] = x[j+1];
        x[j+1] = temp;
      }
    }
  }

  putintarr(x,len);
  putchar('\n');

  puts("fibonacci sequence:");
  fib(15);
  putchar('\n');

  puts("factorial (recursion)");
  putint(factorial(6));

  char *s;
  s = "press any key to continue (getchar()) ";
  putchar('\n');
  putstr(s);
  int c = getchar();
  putstr("you inputted the character with value ");
  putint(c);
  putchar('\n');
  putchar('\n');


  puts("demonstration of goto:");
  int k = 0;
  for(i = 0; i < 5; i++)
  {
    for(j = 0; j < 5; j++)
    {
      k++;
      putint(k);
      putchar('\n');

      if(k == 13)
      {
        puts("exiting nested loop");
        goto endloop;
      }
    }
  }
endloop:
  puts("hi");

  puts("print every number not divisible by 6 (continue):");
  int f, g;
  for(f = 1; f < 30; f++)
  {
    if(f % 6 == 0)
    {
      continue;
    }
    putint(f);
    putchar(' ');
  }
  putchar('\n');

  for(f = 0; i < 10; f++)
  {
    for(g = 0; g < 10; g++)
    {
      int n = f*10 + g;
      if(n % 7 == 0)
        break;
      putint(n);
      putchar(' ');
    }

    putchar('\n');
    if(n % 7 == 0)
      break;
  }
  putchar('\n');
}

void fib(int n)
{
  int i;
  int a = 1;
  int b = 1;
  int temp;
  for(i = 0; i < n; i++)
  {
    putint(a);
    temp = b;
    b = a + b;
    a = temp;
    putchar('\n');
  }
}

int factorial(int n)
{
  if(n == 0)
    return 1;
  else
  {
    return factorial(n-1) * n;
  }
}
