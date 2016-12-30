int BLOCK=0;
int main () {
  int inp1, inp2, inp3; int least = inp1;
  int most = inp1;
  if (most < inp2)
    most = inp2; // C1
  if (most < inp3) most = inp3; // C2
  if (least > inp2)
    most = inp2; // C3 (fault#1)
  if (least > inp3) 
    least = inp3; // C4
  //if(2<3)
  //{
  //  least=inp3;
  //}

  //_Bool flag;
  //if(flag) assert(!(least<=most));
  //else assert(least<=most); //failure
  ////if(1)
  assert(least<=most); //failure

}
