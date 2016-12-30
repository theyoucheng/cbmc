#define FAIL
#include <assert.h>

int BLOCK1 = 0;
int BLOCK2 = 0;
int BLOCK3 = 0;
int BLOCK4 = 0;
int BLOCK5 = 0;
int BLOCK6 = 0;
int BLOCK7 = 0;
int BLOCK8 = 0;
int BLOCK9 = 0;
int BLOCK10 = 0;
int BLOCK11 = 0;
int BLOCK12 = 0;
int BLOCK13 = 0;
int BLOCK14 = 0;
int BLOCK15 = 0;
int BLOCK16 = 0;
int BLOCK17 = 0;
int BLOCK18 = 0;
int BLOCK19 = 0;
int BLOCK20 = 0;
int BLOCK21 = 0;
int BLOCK22 = 0;
int BLOCK23 = 0;
int BLOCK24 = 0;
int BLOCK25 = 0;
int BLOCK26 = 0;
int BLOCK27 = 0;
int BLOCK28 = 0;
int BLOCK29 = 0;
int BLOCK30 = 0;
int BLOCK31 = 0;
int BLOCK32 = 0;
int BLOCK33 = 0;
int BLOCK34 = 0;
int BLOCK35 = 0;
int BLOCK36 = 0;
int BLOCK37 = 0;
int BLOCK38 = 0;
int BLOCK39 = 0;
int BLOCK40 = 0;
int BLOCK41 = 0;
int BLOCK42 = 0;
int BLOCK43 = 0;
int BLOCK44 = 0;
int BLOCK45 = 0;
int BLOCK46 = 0;
int BLOCK47 = 0;
int BLOCK48 = 0;
int BLOCK49 = 0;
int BLOCK50 = 0;
int BLOCK51 = 0;
int BLOCK52 = 0;
int BLOCK53 = 0;
int BLOCK54 = 0;
int BLOCK55 = 0;
int BLOCK56 = 0;
int BLOCK57 = 0;
int BLOCK58 = 0;
int BLOCK59 = 0;
int BLOCK60 = 0;
int BLOCK61 = 0;
int BLOCK62 = 0;
int BLOCK63 = 0;
int BLOCK64 = 0;
int BLOCK65 = 0;
int BLOCK66 = 0;
int BLOCK67 = 0;
int BLOCK68 = 0;
int BLOCK69 = 0;
int BLOCK70 = 0;
int BLOCK71 = 0;
int BLOCK72 = 0;
int BLOCK73 = 0;
int BLOCK74 = 0;
int BLOCK75 = 0;

int get_exit_nondet()
{   BLOCK1 = 1;
    int retval;
    return (retval);
}

int main()
{   
  if(1) BLOCK2 = 1;
    int p1;  // condition variable
    int lk1; // lock variable

    int p2;  // condition variable
    int lk2; // lock variable

    int p3;  // condition variable
    int lk3; // lock variable

    int p4;  // condition variable
    int lk4; // lock variable

    int p5;  // condition variable
    int lk5; // lock variable

    int p6;  // condition variable
    int lk6; // lock variable

    int p7;  // condition variable
    int lk7; // lock variable

    int p8;  // condition variable
    int lk8; // lock variable

    int p9;  // condition variable
    int lk9; // lock variable

    int p10;  // condition variable
    int lk10; // lock variable

    int p11;  // condition variable
    int lk11; // lock variable

    int p12;  // condition variable
    int lk12; // lock variable

    int p13;  // condition variable
    int lk13; // lock variable

    int p14;  // condition variable
    int lk14; // lock variable


    int cond;

    //while(1)
    for(unsigned i=0; i<100; ++i)
    {  BLOCK3 = 1;
        cond = nondet_int();
        if (cond == 0) {  BLOCK4 = 1;
            goto out;
        } //else {  BLOCK5 = 1;}
        lk1 = 0; // initially lock is open

        lk2 = 0; // initially lock is open

        lk3 = 0; // initially lock is open

        lk4 = 0; // initially lock is open

        lk5 = 0; // initially lock is open

        lk6 = 0; // initially lock is open

        lk7 = 0; // initially lock is open

        lk8 = 0; // initially lock is open

        lk9 = 0; // initially lock is open

        lk10 = 0; // initially lock is open

        lk11 = 0; // initially lock is open

        lk12 = 0; // initially lock is open

        lk13 = 0; // initially lock is open

        lk14 = 0; // initially lock is open


    // lock phase
        if (p1 != 0) {  BLOCK6 = 1;
            lk1 = 1; // acquire lock
        } //else {  BLOCK7 = 1;}

        if (p2 != 0) {  BLOCK8 = 1;
            lk2 = 1; // acquire lock
        } //else {BLOCK9 = 1;}

        if (p3 != 0) {BLOCK10 = 1;
            lk3 = 1; // acquire lock
        } //else {BLOCK11 = 1;}

        if (p4 != 0) {BLOCK12 = 1;
            lk4 = 1; // acquire lock
        } //else {BLOCK13 = 1;}

        if (p5 != 0) {BLOCK14 = 1;
            lk5 = 1; // acquire lock
        } //else {BLOCK15 = 1;}

        if (p6 != 0) {BLOCK16 = 1;
            lk6 = 1; // acquire lock
        } //else {BLOCK17 = 1;}

        if (p7 != 0) {BLOCK18 = 1;
            lk7 = 1; // acquire lock
        } //else {BLOCK19 = 1;}

        if (p8 != 0) {BLOCK20 = 1;
            lk8 = 1; // acquire lock
        } //else {BLOCK21 = 1;}

        if (p9 != 0) {BLOCK22 = 1;
            lk9 = 1; // acquire lock
        } //else {BLOCK23 = 1;}

        if (p10 != 0) {BLOCK24 = 1;
            lk10 = 1; // acquire lock
        } //else {BLOCK25 = 1;}

        if (p11 != 0) {BLOCK26 = 1;
            lk11 = 1; // acquire lock
        } //else {BLOCK27 = 1;}

        if (p12 != 0) {BLOCK28 = 1;
            lk12 = 1; // acquire lock
        } //else {BLOCK29 = 1;}

        if (p13 != 0) {BLOCK30 = 1;
            lk13 = 1; // acquire lock
        } //else {BLOCK31 = 1;}

        if (p14 != 0) {BLOCK32 = 1;
            lk14 = 1; // acquire lock
        } //else {BLOCK33 = 1;}


    // unlock phase
        if (p1 != 0) { BLOCK34 = 1;
            if (lk1 != 1) {BLOCK35 = 1; goto ERROR;} // assertion failure
            lk1 = 0;
        } //else {BLOCK36 = 1;}

        if (p2 != 0) { BLOCK37 = 1;
            if (lk2 != 1) {BLOCK38 = 1; goto ERROR;} // assertion failure
            lk2 = 0;
        } else {BLOCK39 = 1; goto ERROR;} // BUG1

        if (p3 != 0) {BLOCK40 = 1;
            if (lk3 != 1) {BLOCK41 = 1;goto ERROR;} // assertion failure
            lk3 = 0;
        } //else {BLOCK42 = 1;}

        if (p4 != 0) {BLOCK43 = 1;
            if (lk4 != 1) {BLOCK44 = 1;goto ERROR;} // assertion failure
            lk4 = 0;
        } //else {BLOCK45 = 1;}

        if (p5 != 0) {BLOCK46 = 1;
            if (lk5 != 1) {BLOCK47 = 1;goto ERROR;} // assertion failure
            lk5 = 0;
        } //else {BLOCK48 = 1;}

        if (p6 != 0) {BLOCK49 = 1;
            if (lk6 != 1) {BLOCK50 = 1;goto ERROR;} // assertion failure
            lk6 = 0;
        } //else {BLOCK51 = 1;}

        if (p7 != 0) {BLOCK52 = 1;
            if (lk7 != 1) {BLOCK53 = 1;goto ERROR;} // assertion failure
            lk7 = 0;
        } //else {BLOCK54 = 1;}

        if (p8 != 0) {BLOCK55 = 1;
            if (lk8 != 1) {BLOCK56 = 1; goto ERROR;} // assertion failure
            lk8 = 0;
        } //else {BLOCK57 = 1;}

        if (p9 != 0) {BLOCK58 = 1;
            if (lk9 != 1) {BLOCK59 = 1; goto ERROR;} // assertion failure
            lk9 = 0;
        } //else {BLOCK60 = 1;}

        if (p10 != 0) {BLOCK61 = 1;
            if (lk10 != 1) {BLOCK62 = 1; goto ERROR;} // assertion failure
            lk10 = 0;
        } //else {BLOCK63 = 1;}

        if (p11 != 0) {BLOCK64 = 1;
            if (lk11 != 1) {BLOCK65 = 1; goto ERROR;} // assertion failure
            lk11 = 0;
        } //else {BLOCK66 = 1;}

        if (p12 != 0) {BLOCK67 = 1;
            if (lk12 != 1) {BLOCK68 = 1; goto ERROR;} // assertion failure
            lk12 = 0;
        } //else {BLOCK69 = 1;}

        if (p13 != 0) {BLOCK70 = 1;
            if (lk13 != 1) {BLOCK71 = 1; goto ERROR;} // assertion failure
            lk13 = 0;
        } //else {BLOCK72 = 1;}

        if (p14 != 0) {BLOCK73 = 1;
            if (lk14 != 1) {BLOCK74 = 1; goto ERROR;} // assertion failure
            lk14 = 0;
        } else {BLOCK75 = 1; goto ERROR;} // BUG2

    }

    // BUGS 75 and 39 BLOCK75==1 | BLOCK39==1
  out:
        
  #ifdef PASS
  //assert(0);//SUT
  #endif
  //  return 0;
  
  ERROR: 
    if(BLOCK39==0)
      assert(0);
  //    __CPROVER_fault=1;
  ////#ifdef FAIL
  //_Bool flag1, flag2;
  //if(flag1)
  //  assert(BLOCK39==0);
  //else
  //  assert(BLOCK39!=0);

  //#endif
    return 0;  
    
}

