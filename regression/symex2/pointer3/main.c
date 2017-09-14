
int main()
{
  int choice;
  int x=1, y=2, *p=choice?&x:&y;

  *p=3;

  if(choice)
    __CPROVER_assert(x==3 && y==2, "");
  else
    __CPROVER_assert(x==1 && y==3, "");
}
