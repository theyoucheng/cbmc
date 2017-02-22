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

int main()
{
  int symbol1;
  unsigned x, ___result___;
  int y;
  //float z;
  double z, t;
  int m, n, res;
_Bool A, B, C;
  
  


  //__AUTOSAC_postcondition(A, "autosc post conditions");
  //__AUTOSAC_postcondition(A&&B, "autosc post conditions");
  //__AUTOSAC_postcondition(A&&B&&C, "autosc post conditions");
  //__AUTOSAC_postcondition(A||B, "autosc post conditions");
  //__AUTOSAC_postcondition(A||B||C, "autosc post conditions");
  //__AUTOSAC_postcondition(x==y, "autosc post conditions");
  //__AUTOSAC_postcondition(A&&x!=2, "autosc post conditions");
  //__AUTOSAC_postcondition(x!=2, "autosc post conditions");
  //__AUTOSAC_postcondition(x!=y, "autosc post conditions");
  //__AUTOSAC_postcondition(x!=y&&m>n, "autosc post conditions");
  //__AUTOSAC_postcondition(m+n>10&&x==10, "autosc post conditions");
  //__AUTOSAC_postcondition(m+n>10, "autosc post conditions");
  //__AUTOSAC_postcondition(res==0, "autosc post conditions");
  //__AUTOSAC_postcondition((A?m:n)&&(B?x:y), "autosc post conditions");
  //__AUTOSAC_postcondition(res==(A?m:n), "autosc post conditions");
  //__AUTOSAC_postcondition((A?2:1), "autosc post conditions");
  // This is a bug __AUTOSAC_postcondition(((!A)?2:1), "autosc post conditions");
  //__AUTOSAC_postcondition((A?m:n), "autosc post conditions");
  //__AUTOSAC_postcondition(A, "autosc post conditions");

  __AUTOSAC_postcondition(0<=m && m<=10, "autosc post conditions");
  
  //__AUTOSAC_postcondition(m+n>10000?res==10000:res==m+n, "autosc post conditions");
  __AUTOSAC_conditions_go_here("postconditions");
  
  //__CPROVER_assert(f_expr(x,y)<z, "");
  //__AUTOSAC_postcondition(f_expr(ab1, ab2) && z>10, "autosc post conditions");
  //__AUTOSAC_postcondition(x>y && z>10, "autosc post conditions");

  //__AUTOSAC_conditions_go_here("preconditions");


  return 1;
}
