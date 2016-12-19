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

int main()
{
  int symbol1;
  unsigned x, ___result___;
  int y;
  //float z;
  double z;
  
  __CPROVER_input("x", x);
  __CPROVER_input("y", y);
  __CPROVER_input("z", z);

  

  __AUTOSAC_in_type((x+y)>=1 && (x+y)<=10, "x is in type");
  //__AUTOSAC_in_type(x>=1 && x<=10, "x is in type");
  //__AUTOSAC_in_type(y>=1 && y<=10, "y is in type");
  __AUTOSAC_in_type(z>=1 && z<=10, "z is in type");
  //__AUTOSAC_in_type(y>=2 && y<=5, "y is in type");
  //__AUTOSAC_conditions_go_here("preconditions");
  
  //if (!(x>3) && y<5) 
  //  ;

  //if(___result___==(x>y? x<3:y>2));
  //if((x>y? x<3:y>2));
  //__AUTOSAC_in_type(___result___==(x>y? x<3:y>2), "in type");

  //int i;
  //int a[4];
  ////__AUTOSAC_postcondition(((___result___ >= 1 && ___result___ <= 4)) ? ((a)[___result___ - 1] == i) : __CPROVER_forall { unsigned index; (!(index >= 1 && index <= 4)) || (a)[index - 1] != i },
  //__AUTOSAC_in_type(((___result___ >= 1 && ___result___ <= 4)) ? ((a)[___result___ - 1] == i) : __CPROVER_forall { unsigned index; (!(index >= 1 && index <= 4)) || (a)[index - 1] != i },
  //                        "postcondition at binary_search.ads:23:15");

  //__AUTOSAC_in_type((x>y? x<3:y>2), "autosac in type");




  __AUTOSAC_conditions_go_here("preconditions");


  return 1;
}
