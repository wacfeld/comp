void putchar();

int getchar();


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

int main()
{
  int x[10];
  x[0] = 5;
  x[1] = 4;
  x[2] = 7;
  x[3] = 1;
  x[4] = 6;
  x[5] = 5;
  x[6] = 11;
  x[7] = -2;
  x[8] = 0;
  x[9] = 7;

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
    ;
  }
}
