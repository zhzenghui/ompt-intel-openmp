#include <omp.h>

#define MAX (1LL << 30)

void g()
{
#pragma omp parallel 
{
    long long i;
    for(i = 0; i < MAX; i++);
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
