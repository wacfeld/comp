int factorial(int n);

int main()
{
  int x = factorial(6);
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
