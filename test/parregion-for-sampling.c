#include <omp.h>

#define N 42

int fib(int n)
{
  if(n<2) return n;
  else
    return fib(n-1)+fib(n-2);
}

void g()
{
#pragma omp parallel 
{
    fib(N);
}
}

void f()
{
  g();
}

int main()
{
  f();
  return 0;
}
