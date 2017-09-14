#include <assert.h>

int main()
{
  int *p, i;
  char *q;
  char ch0, ch1, ch2, ch3;

  // pointer into an integer
  q=(char *)&i;

  i=0x03020100;

  // endianness-dependent
  ch0=q[0];
  ch1=q[1];
  ch2=q[2];
  ch3=q[3];

  __CPROVER_assert(ch0==0, "");
  __CPROVER_assert(ch1==1, "");
  __CPROVER_assert(ch2==2, "");
  __CPROVER_assert(ch3==3, "");

  unsigned int *up=(unsigned int *)q;
  __CPROVER_assert(*up==i, "");

  // pointer with conditional
  int input, x, y;
  p=input?&x:&y;
  *p=1;

  if(input)
    __CPROVER_assert(x==1, "");
  else
    __CPROVER_assert(y==1, "");
}
