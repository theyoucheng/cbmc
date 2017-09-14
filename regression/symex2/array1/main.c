
struct some_struct
{
  int x, y, z;
} some_struct_array[10];

int some_int_array[10];

int main()
{
  int i;

  // zero initialization
  __CPROVER_assert(some_int_array[1]==0, "");
  if(i>=0 && i<10) __CPROVER_assert(some_int_array[i]==0, "");

  some_int_array[5]=5;
  __CPROVER_assert(some_int_array[1]==0, "");
  __CPROVER_assert(some_int_array[5]==5, "");
}
