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

struct AB
{
  int a;
  int b;
};

int f_expr(struct AB xy1, struct AB ab2)
{
  __AUTOSAC_expression_function();
  //a++;
  //b--;
  return xy1.a>ab2.a && xy1.b>ab2.b;
}

int main()
{
  int symbol1;
  unsigned x, ___result___;
  int y;
  //float z;
  double z;
  
//  __CPROVER_input("x", x);
//  __CPROVER_input("y", y);
//  __CPROVER_input("z", z);
//  int w=sqrt(100);

  //f_expr(x, y);
  

  struct AB ab1, ab2;
  __CPROVER_input("ab1", ab1);
  __CPROVER_input("ab2", ab2);

  //__CPROVER_assert(f_expr(x,y)<z, "");
  __AUTOSAC_postcondition(f_expr(ab1, ab2) && z>10, "autosc post conditions");
  //__AUTOSAC_postcondition(x>y && z>10, "autosc post conditions");

  //__AUTOSAC_conditions_go_here("preconditions");


  return 1;
}
