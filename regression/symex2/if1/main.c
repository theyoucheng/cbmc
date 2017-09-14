
int main()
{
  int i;

  if(i==1)
    __CPROVER_assert(i==1, "");
  else
    __CPROVER_assert(i!=1, "");

  __CPROVER_assert(i!=1, "");
}
