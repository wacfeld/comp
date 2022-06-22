void putchar(int x);

int getchar();
void exit();


void putint(int x)
{
  if(x < 0)
  {
    putchar('-');
    x = -x;
  }

  if(x == 0)
  {
    putchar('0');
    return;
  }

  char arr[100];
  int i = 0;
  
  while(x)
  {
    arr[i++] = x%10 + '0';
    x = x / 10;
  }
  
  for(i--; i >= 0; i--)
  {
    putchar(arr[i]);
  }
}

void fib(int n);
int factorial(int x);

int thing(int x, char y)
{
  return x + y;
}

int main()
{
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

  for(i = 0; i < 10; i++)
  {
    putint(x[i]);
    putchar('\n');
  }

  putchar('\n');
  fib(15);

  putint(factorial(6));
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

int add1(int n)
{
  return n + 1;
}

int factorial(int n)
{
  if(n == 0)
    return 1;
  else
  {
    int f = factorial(n-1);
    return f * n;
  }
}
