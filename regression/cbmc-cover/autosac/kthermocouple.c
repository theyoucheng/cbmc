#include <stdio.h>
#include <math.h>
//void __AUTOSAC_in_type(const unsigned b, const char* c)
void __AUTOSAC_in_type(const __CPROVER_bool b, const char* c)
{
}
void __AUTOSAC_postcondition(const __CPROVER_bool b, const char* c)
{
}
void __AUTOSAC_conditions_go_here(const char* c)
{
}
void __AUTOSAC_empty()
{
}

//struct AB
//{
//  int a;
//  int b;
//};
//
//int f_expr(struct AB xy1, struct AB ab2)
//{
//  __AUTOSAC_expression_function();
//  //a++;
//  //b--;
//  return xy1.a>ab2.a && xy1.b>ab2.b;
//}

int f()
{
  int u;
  return u;
}


int main()
{
  int symbol1;
  unsigned x, ___result___;
  int y;
  //float z;
  double z, t;
  int m, n, res;
   int a,b, c;
_Bool A, B, C;
  
  


  __AUTOSAC_postcondition((f()==(a-b)*c) && (A?a==1:b==1), "post"); //f()<=1?res==2:(f()<2?res==1:res==3), "autosc post conditions");
  //__AUTOSAC_postcondition(A&& B, "post"); //f()<=1?res==2:(f()<2?res==1:res==3), "autosc post conditions");
  //__AUTOSAC_postcondition((f()==(a-b)*c) && f()<=1?res==2:(f()<2?res==1:res==3), "autosc post conditions");

  __AUTOSAC_conditions_go_here("postconditions");
  

  return 1;
}
