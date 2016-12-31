int __CPROVER_fault387=1;
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "faults.h"

#define OLEV       600		/* in feets/minute */
#define MAXALTDIFF 600		/* max altitude difference in feet */
#define MINSEP     300      /* min separation in feet */
#define NOZCROSS   100		/* in feet */


// coverage blocks
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


int nondet_int (void);

//int assert();
int p1a, p1b, p2a, p2b, p3a, p3b, p4a, p4b, p5a, p5b;
int Cur_Vertical_Sep;
int High_Confidence;
int Two_of_Three_Reports_Valid;
int Own_Tracked_Alt;
int Own_Tracked_Alt_Rate;
int Other_Tracked_Alt;
int Alt_Layer_Value;
int Positive_RA_Alt_Thresh_0;
int Positive_RA_Alt_Thresh_1;
int Positive_RA_Alt_Thresh_2;
int Positive_RA_Alt_Thresh_3;
int Up_Separation;
int Down_Separation;
int Other_RAC;
int Other_Capability;
int Climb_Inhibit;


int Own_Below_Threat()
{ BLOCK1=1;
#ifdef FAULT_v6
    return (Own_Tracked_Alt <= Other_Tracked_Alt); /* operator mutation */ /* LOCATION_v6 */
#else
#ifdef FAULT_v10a
    /* NOTE JMW replaced LESS-THAN with LESS-THAN-EQUAL. */
    return (Own_Tracked_Alt <= Other_Tracked_Alt); /* LOCATION_v10a */
#else
    return (Own_Tracked_Alt < Other_Tracked_Alt);
#endif
#endif
}

int Own_Above_Threat()
{ BLOCK2=1;
#ifdef FAULT_v10b
    /* NOTE JMW replaced LESS-THAN with LESS-THAN-EQUAL. */
    return (Other_Tracked_Alt <= Own_Tracked_Alt); /* LOCATION_v10b */
#else
    return (Other_Tracked_Alt < Own_Tracked_Alt);
#endif
}

int initialize()
{ BLOCK3=1;
  High_Confidence = 32767;
  Other_Capability = 0;


  Climb_Inhibit = 1;
  Cur_Vertical_Sep = 16684;
  Own_Tracked_Alt_Rate = 450 ;

   //assert(Own_Tracked_Alt >= 0);
   //assert(Other_Tracked_Alt >= 0);
   //assert(Own_Tracked_Alt_Rate >= 0);

  Positive_RA_Alt_Thresh_0 = 16434;
  Positive_RA_Alt_Thresh_1 = 0;
  Positive_RA_Alt_Thresh_2 = 0;
  Positive_RA_Alt_Thresh_3 = 0;
  return 1;
}

int Positive_RA_Alt_Thresh(int Alt)
{ BLOCK4=1;
  int res = 0;
  if( Alt == 0 )
    { res = Positive_RA_Alt_Thresh_0; }
  if( Alt == 1 )
    { res = Positive_RA_Alt_Thresh_1; }
  if( Alt == 2 )
    { res = Positive_RA_Alt_Thresh_2; }
  if( Alt == 3 )
    { res = Positive_RA_Alt_Thresh_3; }
  return(res);
}

int ALIM ()
{ BLOCK5=1;
#ifdef FAULT_v37
 return Positive_RA_Alt_Thresh_0; /* LOCATION_v37 */
#else
 return Positive_RA_Alt_Thresh(Alt_Layer_Value);
#endif
}

int Inhibit_Biased_Climb ()
{ BLOCK6=1;
#ifdef FAULT_v2
    return (Climb_Inhibit ? Up_Separation + MINSEP /* operand mutation NOZCROSS */ : Up_Separation); /* LOCATION_v2 */
#else
#ifdef FAULT_v28
    return ((Climb_Inhibit == 0) ? Up_Separation + NOZCROSS : Up_Separation); /* LOCATION_v28 */
#else
#ifdef FAULT_v29
    return (Up_Separation); /* LOCATION_v29 */
#else
#ifdef FAULT_v30
    return (Up_Separation + NOZCROSS); /* LOCATION_v30 */
#else
#ifdef FAULT_v35
    return (Climb_Inhibit ? Up_Separation : Up_Separation + NOZCROSS); /* LOCATION_v35 */
#else
    return (Climb_Inhibit ? Up_Separation + NOZCROSS : Up_Separation);
#endif
#endif
#endif
#endif
#endif
}

int Non_Crossing_Biased_Climb()
{ BLOCK7=1;
  int upward_preferred;
  int upward_crossing_situation;
  int result;

  #ifdef FAULT_v20
    upward_preferred = Inhibit_Biased_Climb() >= Down_Separation; /* LOCATION_v20 */
#else
#ifdef FAULT_v21
    upward_preferred = (Up_Separation + NOZCROSS) > Down_Separation; /* LOCATION_v21 */
#else
#ifdef FAULT_v22
    upward_preferred = Up_Separation > Down_Separation; /* LOCATION_v22 */
#else
    upward_preferred = Inhibit_Biased_Climb() > Down_Separation;
#endif
#endif
#endif
  
  if (upward_preferred)
    { BLOCK8=1;
#ifdef FAULT_v1
	result = !(Own_Below_Threat()) || ((Own_Below_Threat()) && (!(Down_Separation > ALIM()))); /* opertor mutation */ /* LOCATION_v1 */ //P3B
#else
#ifdef FAULT_v40a
	result = ((Own_Below_Threat()) && (!(Down_Separation >= ALIM()))); /* LOCATION_v40a */
#else
	result = !(Own_Below_Threat()) || ((Own_Below_Threat()) && (!(Down_Separation >= ALIM()))); //
#endif
#endif
#ifdef FAULT_v31a
	result = result && (Own_Tracked_Alt <= Other_Tracked_Alt); /* LOCATION_v31a */
#endif
    }
  else
    { BLOCK9=1;
#ifdef FAULT_v4
	result = Own_Above_Threat() && (Cur_Vertical_Sep >= MINSEP) || (Up_Separation >= ALIM()); /* logic change */ /* LOCATION_v4 */
#else
#ifdef FAULT_v41
	result = (Cur_Vertical_Sep >= MINSEP) && (Up_Separation >= ALIM()); /* LOCATION_v41 */
#else
	result = Own_Above_Threat() && (Cur_Vertical_Sep >= MINSEP) && (Up_Separation >= ALIM());
#endif
#endif
#ifdef FAULT_v31b
	result = result && (Own_Tracked_Alt < Other_Tracked_Alt); /* LOCATION_v31b */
#endif
    }
  return result;
}

int Non_Crossing_Biased_Descend()
{ BLOCK10=1;
  int upward_preferred;
  int upward_crossing_situation;
  int result;

#ifdef FAULT_v9
    upward_preferred = Inhibit_Biased_Climb() >= Down_Separation; /* operator mutation */ /* LOCATION_v9 */
#else
#ifdef FAULT_v23
    upward_preferred = (Up_Separation + NOZCROSS) > Down_Separation; /* LOCATION_v23 */
#else
#ifdef FAULT_v24
    upward_preferred = Up_Separation > Down_Separation; /* LOCATION_v24 */
#else
    upward_preferred = Inhibit_Biased_Climb() > Down_Separation;
#endif
#endif
#endif
	
  if (upward_preferred)
    { BLOCK11=1;
      result = Own_Below_Threat() && (Cur_Vertical_Sep >= 300 ) && (Down_Separation >= ALIM());
#ifdef FAULT_v32a
	result = result && (Other_Tracked_Alt < Own_Tracked_Alt); /* LOCATION_v32a */
#endif
    }
  else
    { BLOCK12=1;
#ifdef FAULT_v25
	result = !(Own_Above_Threat()) || ((Own_Above_Threat()) && (Up_Separation > ALIM())); /* LOCATION_v25 */
#else
#ifdef FAULT_v39
	result = !(Own_Above_Threat()) || ((Own_Above_Threat()) && (Up_Separation > ALIM())); /* LOCATION_v39 */
#else
	result = !(Own_Above_Threat()) || ((Own_Above_Threat()) && (Up_Separation >= ALIM()));
#endif
#endif
#ifdef FAULT_v32b
	result = result && (Other_Tracked_Alt <= Own_Tracked_Alt); /* LOCATION_v32b */
#endif
    }
  return result;
}


/*@
  @ behavior P1a :
  @   assumes
  @      Up_Separation >= Positive_RA_Alt_Thresh_2 &&
  @      Down_Separation < Positive_RA_Alt_Thresh_2;
  @   ensures
  @      \result != 2;
  @ behavior P1b :
  @   assumes
  @      Up_Separation < Positive_RA_Alt_Thresh_2 &&
  @      Down_Separation >= Positive_RA_Alt_Thresh_2;
  @   ensures
  @      \result != 1;
  @
  @ behavior P2a :
  @   assumes
  @      Up_Separation < Positive_RA_Alt_Thresh_2 &&
  @      Down_Separation < Positive_RA_Alt_Thresh_2 &&
  @      Up_Separation > Down_Separation;
  @   ensures
  @      \result != 2;
  @ behavior P2b :
  @   assumes
  @      Up_Separation < Positive_RA_Alt_Thresh_2 &&
  @      Down_Separation < Positive_RA_Alt_Thresh_2 &&
  @      Up_Separation < Down_Separation;
  @   ensures
  @      \result != 1;
  @
  @ behavior P3a :
  @   assumes
  @      Up_Separation >= Positive_RA_Alt_Thresh_2 &&
  @      Down_Separation >= Positive_RA_Alt_Thresh_2 &&
  @      Own_Tracked_Alt > Other_Tracked_Alt;
  @   ensures
  @      \result != 2;
  @ behavior P3b :
  @   assumes
  @      Up_Separation >= Positive_RA_Alt_Thresh_2 &&
  @      Down_Separation >= Positive_RA_Alt_Thresh_2 &&
  @      Own_Tracked_Alt < Other_Tracked_Alt;
  @   ensures
  @      \result != 1;
  @
  @ behavior P4a :
  @   assumes
  @      Own_Tracked_Alt > Other_Tracked_Alt;
  @   ensures
  @      \result != 2;
  @ behavior P4b :
  @   assumes
  @      Own_Tracked_Alt < Other_Tracked_Alt;
  @   ensures
  @      \result != 1;
  @
  @ behavior P5a :
  @   assumes
  @      Up_Separation > Down_Separation;
  @   ensures
  @      \result != 2;
  @ behavior P5b :
  @   assumes
  @      Up_Separation < Down_Separation;
  @    ensures
  @      \result != 1;
  @*/

int alt_sep_test()
{ BLOCK13=1;
  int enabled, tcas_equipped, intent_not_known;
  int need_upward_RA = 0;
  int need_downward_RA = 0;

  int alt_sep;
  
  //assert (Own_Tracked_Alt >= 300);
  //assert (Other_Tracked_Alt >= 300);
  //assert (Own_Tracked_Alt_Rate >= 300);

#ifdef FAULT_v5
    enabled = High_Confidence && (Own_Tracked_Alt_Rate <= OLEV); /* && (Cur_Vertical_Sep > MAXALTDIFF); missing code */ /* LOCATION_v5 */
#else
#ifdef FAULT_v12
    enabled = High_Confidence || (Own_Tracked_Alt_Rate <= OLEV) && (Cur_Vertical_Sep > MAXALTDIFF); /* LOCATION_v12 */
#else
#ifdef FAULT_v26
    enabled = High_Confidence && (Cur_Vertical_Sep > MAXALTDIFF); /* LOCATION_v26 */
#else
#ifdef FAULT_v27
    enabled = High_Confidence && (Own_Tracked_Alt_Rate <= OLEV) ; /* LOCATION_v27 */
#else
    enabled = High_Confidence && (Own_Tracked_Alt_Rate <= OLEV) && (Cur_Vertical_Sep > MAXALTDIFF);
#endif
#endif
#endif
#endif
  
  tcas_equipped = Other_Capability == 1 ;
  
#ifdef FAULT_v3
    intent_not_known = Two_of_Three_Reports_Valid || Other_RAC == 0; /* logic change */ /* LOCATION_v3 */
#else
    intent_not_known = Two_of_Three_Reports_Valid && Other_RAC == 0;
#endif

  alt_sep = 0 ;
  
#ifdef FAULT_v34
    if (enabled && tcas_equipped && intent_not_known || !tcas_equipped) /* LOCATION_v34 */
#else
    if (enabled && ((tcas_equipped && intent_not_known) || !tcas_equipped))
#endif
    { BLOCK14=1;
      #ifdef FAULT_v31c
	need_upward_RA = Non_Crossing_Biased_Climb(); /* LOCATION_v31c */
#else
#ifdef FAULT_v40b
	need_upward_RA = Non_Crossing_Biased_Climb(); /* LOCATION_v40b */
#else
	need_upward_RA = Non_Crossing_Biased_Climb() && Own_Below_Threat();
#endif
#endif
#ifdef FAULT_v32c
	need_downward_RA = Non_Crossing_Biased_Descend(); /* LOCATION_v32c */
#else
	need_downward_RA = Non_Crossing_Biased_Descend() && Own_Above_Threat();
#endif

      
      // ****************************** SPECIFICATIONS ******************************************
      
      if(need_upward_RA && need_downward_RA) 
        { 
          alt_sep = 0 ;
           
          #ifndef FAIL_spec   
          //assert (0);// SUT
          
          #endif
           
        }
      else 
      {
if(!(need_upward_RA||need_downward_RA)) __CPROVER_fault387=0;_Bool flag387;  if(flag387) __CPROVER_assert(__CPROVER_fault387==0, "__CPROVER_fault passing traces: line 387");  else  __CPROVER_assert(__CPROVER_fault387==1, "__CPROVER_fault failing traces: line 387");
        if (need_upward_RA)
          {
            if (p1b || p2b || p3b || p4b || p5b)
            {  
            #ifdef FAIL_spec   
            assert (0);//SUT
            
            #endif
             
            alt_sep = 1 ;
            }
          }
        else if (need_downward_RA)
          {
            if (p1a || p2a || p3a || p4a || p5a)
            {
            #ifdef FAIL_spec   
            assert (0);//SUT
            
            #endif
             
            alt_sep = 2 ;
            }
          }
        //else 
        //  alt_sep = 0 ;
        //  #ifndef FAIL_spec   
        //  assert(0);//SUT
          
        //  #endif      
      }
    }

    // ********************************* END SPECIFICATIONS ***********************************
    
  return alt_sep;
}

int main(int argc, char *argv[])
{if(1) {int __CPROVER_dummy=1;} BLOCK15=1;
  Cur_Vertical_Sep = nondet_int ();
  High_Confidence = nondet_int ();
  Two_of_Three_Reports_Valid = nondet_int ();
  Own_Tracked_Alt = nondet_int ();
  Own_Tracked_Alt_Rate = nondet_int ();
  Other_Tracked_Alt = nondet_int ();
  Alt_Layer_Value = nondet_int ();
  Positive_RA_Alt_Thresh_0 = nondet_int ();
  Positive_RA_Alt_Thresh_1 = nondet_int ();
  Positive_RA_Alt_Thresh_2 = nondet_int ();
  Positive_RA_Alt_Thresh_3 = nondet_int ();
  Up_Separation = nondet_int ();
  Down_Separation = nondet_int ();
  Other_RAC = nondet_int ();
  Other_Capability = nondet_int ();
  Climb_Inhibit = nondet_int ();

  // enable assertion verification
  p1a = p1b = p2a = p2b = p3a = p3b = p4a = p4b = p5a = p5b = 0;

  // nondet initialization
  int res;

  // general assumptions given by assertions in the program
  __CPROVER_assume (Own_Tracked_Alt >= 0);
  __CPROVER_assume (Other_Tracked_Alt >= 0);
  initialize();

  /* Prop P1A -- verified by CBMC -- unless Up_Sep + 100 ovflws */
  #ifdef P1a
  p1a = 1;
  __CPROVER_assume (Up_Separation >= Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Down_Separation < Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Up_Separation >= Positive_RA_Alt_Thresh_2 );
  __CPROVER_assume (Down_Separation < Positive_RA_Alt_Thresh_2);
  __CPROVER_assume (Own_Tracked_Alt >= 300);
  __CPROVER_assume (Other_Tracked_Alt >= 300);
  __CPROVER_assume (Up_Separation < 2147483647-99);
  res = alt_sep_test();
  p1a = 0;
 #endif

  /* Prop P1B -- verified by CBMC -- OK */
  #ifdef P1b
  p1b = 1;
  __CPROVER_assume (p1b);
  __CPROVER_assume (Up_Separation < Positive_RA_Alt_Thresh_2);
  __CPROVER_assume (Down_Separation >= Positive_RA_Alt_Thresh_2);
  __CPROVER_assume (Up_Separation < Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Up_Separation < 2147483647-99);
  __CPROVER_assume (Down_Separation >= Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Own_Tracked_Alt >= 300);
  __CPROVER_assume (Other_Tracked_Alt >= 300);
  p1a = p2a = p2b = p3a = p3b = p4a = p4b = p5a = p5b = 0;
  res = alt_sep_test();
  p1b = 0;
 #endif

  #ifdef P2a
  /* Prop P2a -- verified by CBMC -- OK */
  p2a = 1;
  __CPROVER_assume (p2a);
  __CPROVER_assume (Up_Separation < Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Down_Separation < Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Up_Separation > Down_Separation);
  __CPROVER_assume (Up_Separation < 2147483647-99);
  __CPROVER_assume (Own_Tracked_Alt >= 300);
  __CPROVER_assume (Other_Tracked_Alt >= 300);
  __CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  p1a = p1b = p2b = p3a = p3b = p4a = p4b = p5a = p5b = 0;
  res = alt_sep_test();
  p2a = 0;
#endif
 
  /* Prop P2B -- fails by CBMC -- OK */
  //p2b = 1;
  //__CPROVER_assume (p2b);
  //__CPROVER_assume (Up_Separation < Positive_RA_Alt_Thresh_0);
  //__CPROVER_assume (Down_Separation < Positive_RA_Alt_Thresh_0);
  //__CPROVER_assume (Up_Separation < Positive_RA_Alt_Thresh_2);
  //__CPROVER_assume (Down_Separation < Positive_RA_Alt_Thresh_2);
  //__CPROVER_assume (Up_Separation < Down_Separation);
  //__CPROVER_assume (Up_Separation < 2147483647-99);
  //__CPROVER_assume (Own_Tracked_Alt >= 300);
  //__CPROVER_assume (Other_Tracked_Alt >= 300);
  //__CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  //p1a = p1b = p2a = p3a = p3b = p4a = p4b = p5a = p5b = 0;
  //res = alt_sep_test();
  //p2b = 0;

  /* Prop P3A -- fails by CBMC -- OK */
  /*
  p3a = 1;
  __CPROVER_assume (p3a);
  __CPROVER_assume (Own_Tracked_Alt >= 300);
  __CPROVER_assume (Other_Tracked_Alt >= 300);
  __CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  __CPROVER_assume (Up_Separation >= Positive_RA_Alt_Thresh_2);
  __CPROVER_assume (Down_Separation >= Positive_RA_Alt_Thresh_2);
  __CPROVER_assume (Up_Separation >= Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Up_Separation < 2147483647-99);
  __CPROVER_assume (Down_Separation >= Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Own_Tracked_Alt > Other_Tracked_Alt);
  p1a = p1b = p2a = p2a = p3b = p4a = p4b = p5a = p5b = 0;
  res = alt_sep_test();
  p3a = 0;
  */

  
  #ifdef P3b 
  /* Prop P3B -- verified by CBMC -- OK*/

  p3b = 1;
  __CPROVER_assume (p3b);
  __CPROVER_assume (Own_Tracked_Alt >= 300);
  __CPROVER_assume (Other_Tracked_Alt >= 300);
  __CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  __CPROVER_assume (Up_Separation >= Positive_RA_Alt_Thresh_2);
  __CPROVER_assume (Down_Separation >= Positive_RA_Alt_Thresh_2);
  __CPROVER_assume (Up_Separation >= Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Down_Separation >= Positive_RA_Alt_Thresh_0);
  __CPROVER_assume (Up_Separation < 2147483647-99);
  __CPROVER_assume (Own_Tracked_Alt < Other_Tracked_Alt);
  p1a = p1b = p2a = p2a = p3a = p4a = p4b = p5a = p5b = 0;
  res = alt_sep_test();
  p3b = 0;
  #endif

  /* Prop P4A -- fails by CBMC -- OK */
  //p4a = 1;
  //__CPROVER_assume (p4a);
  //__CPROVER_assume (Own_Tracked_Alt > Other_Tracked_Alt);
  //__CPROVER_assume (Own_Tracked_Alt >= 300);
  //__CPROVER_assume (Other_Tracked_Alt >= 300);
  //__CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  //p1a = p1b = p2a = p2a = p3a = p3b = p4b = p5a = p5b = 0;
  //res = alt_sep_test();
  //p4a = 0;

  /* Prop P4B -- fails by CBMC -- OK */  
  //p4b = 1;
  //__CPROVER_assume (p4b);
  //__CPROVER_assume (Own_Tracked_Alt < Other_Tracked_Alt);
  //__CPROVER_assume (Own_Tracked_Alt >= 300);
  //__CPROVER_assume (Other_Tracked_Alt >= 300);
  //__CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  //p1a = p1b = p2a = p2a = p3a = p3b = p4a = p5a = p5b = 0;
  //res = alt_sep_test();
  //p4b = 0;

  #ifdef P5a
  /* Prop P5A -- verified by CBMC -- OK if no overflow when Up_Sep + 100 */
  
  p5a = 1;
  __CPROVER_assume (p5a);
  __CPROVER_assume (Up_Separation > Down_Separation);
  __CPROVER_assume (Up_Separation < 2147483647-99);
  __CPROVER_assume (Own_Tracked_Alt >= 300);
  __CPROVER_assume (Other_Tracked_Alt >= 300);
  __CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  p1a = p1b = p2a = p2a = p3a = p3b = p4a = p4b = p5b = 0;
  res = alt_sep_test();
  p5a = 0;
  #endif
 
  /* Prop P5B -- fails by CBMC */
  /*
  p5b = 1;
  __CPROVER_assume (p5b);
  __CPROVER_assume (Up_Separation < Down_Separation);
  __CPROVER_assume (Up_Separation < 2147483647-99);
  __CPROVER_assume (Own_Tracked_Alt >= 300);
  __CPROVER_assume (Other_Tracked_Alt >= 300);
  __CPROVER_assume (Own_Tracked_Alt_Rate >= 300);
  p1a = p1b = p2a = p2a = p3a = p3b = p4a = p4b = p5a = 0;
  res = alt_sep_test();
  p5b = 0;
  */
  
  return 0;
}
