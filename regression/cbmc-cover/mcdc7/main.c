void __AUTOSAC_postcondition(const __CPROVER_bool b, const char* c)
{
}
void __AUTOSAC_conditions_go_here(const char* c)
{
}
int main()
{
  signed x, y;
  
  __CPROVER_input("x", x);
  __CPROVER_input("y", y);

  _Bool A, B, C, D, E;

  __AUTOSAC_postcondition(A?B:C, "autosac postcondition");
  __AUTOSAC_postcondition(x>0?B:(y<=10?D:E), "autosac postcondition");
  __AUTOSAC_postcondition(A?B:(C?D:E), "autosac postcondition");

  __AUTOSAC_conditions_go_here("postconditions");

  return 1;
}
