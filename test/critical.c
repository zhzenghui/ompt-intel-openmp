#include <omp.h>

#include "fib.h"

#define N 10

void g()
{
  int i, j;
  omp_lock_t l;
  omp_init_lock(&l); 

#pragma omp parallel 
  {
    #pragma omp master
    {
      #pragma omp critical (it)
      {
	fib(42);
      }
    }
    #pragma omp for
    for(i = 0; i<100; i++) {
      #pragma omp critical (it)
      {
	fib(N);
      }
    }
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
