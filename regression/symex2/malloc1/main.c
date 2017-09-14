#include <assert.h>
#include <stdlib.h>

int main()
{
  int *p;

  p=(int *)malloc(sizeof(int));

  *p=1;

  __CPROVER_assert(*p==1, "");

  free(p);
}
