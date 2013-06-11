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
    int val = N;
    int n = omp_get_thread_num();

    /* adjust val for imbalanced waiting */
    if (n < 42) val = val-n;

    fib(val);
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
