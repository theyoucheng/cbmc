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
  unsigned m, ___result___, year;
  
//  __CPROVER_input("x", x);
//  __CPROVER_input("y", y);
//  __CPROVER_input("z", z);
//  int w=sqrt(100);

  //f_expr(x, y);
  

  //struct AB ab1, ab2;
  //__CPROVER_input("ab1", ab1);
  //__CPROVER_input("ab2", ab2);

  __AUTOSAC_postcondition(___result___ == ((m == 8 | m == 3 | m == 5 | m == 10) ? 30 : ((m == 1) ? ((year % 100) == 0) ? (((year % 400) == 0) ? 29 : 28) : (((year % 4) == 0) ? 29 : 28) : 31)), "postcondition at basic_tests.ads:72:40");
  //__AUTOSAC_postcondition(m+n>10, "autosc post conditions");
  //__AUTOSAC_postcondition(res==0, "autosc post conditions");
  //__AUTOSAC_postcondition(m+n>10000?res==10000:res==m+n, "autosc post conditions");
  __AUTOSAC_conditions_go_here("postconditions");
  
  //__CPROVER_assert(f_expr(x,y)<z, "");
  //__AUTOSAC_postcondition(f_expr(ab1, ab2) && z>10, "autosc post conditions");
  //__AUTOSAC_postcondition(x>y && z>10, "autosc post conditions");

  //__AUTOSAC_conditions_go_here("preconditions");


  return 1;
}
