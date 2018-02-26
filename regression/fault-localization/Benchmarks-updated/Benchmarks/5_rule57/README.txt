SVCOMP 2013

BUGS (# adjusted line numberS): 49, 75-6, 96, 103

SCORE: Sc=2, F=1, P=4

time ~/cbmc/src/cbmc/cbmc rule57_ebda_blast.c_unsafe.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 1 --unwind-max 1

*** after simplification (blocks) *** 
[-1] ##file files/rule57_ebda_blast.c line 57 function ebda_rsrc_controller ##file files/rule57_ebda_blast.c line 59 function ebda_rsrc_controller ##file files/rule57_ebda_blast.c line 61 function ebda_rsrc_controller 
[4] ##file files/rule57_ebda_blast.c line 47 function ibmphp_find_same_bus_num ##file files/rule57_ebda_blast.c line 65 function ebda_rsrc_controller ##file files/rule57_ebda_blast.c line 74 function ebda_rsrc_controller 
[536] ##file files/rule57_ebda_blast.c line 100 function ebda_rsrc_controller ##file files/rule57_ebda_blast.c line 103 function ebda_rsrc_controller ##file files/rule57_ebda_blast.c line 39 function kfree 
[true] ##file files/rule57_ebda_blast.c line 54 function ebda_rsrc_controller 

failing traces: 
1 1 1 1 

passing traces: 
1 1 1 1 
1 1 0 1 
1 0 0 1 
0 0 0 1 

***after compute spectra***
**ef:
1 1 1 1 
**ep:
3 2 1 4 
**nf:
0 0 0 0 
**np:
1 2 3 0 

***after measure_sb***
0.4 0.6 0.8 0.2 


** Most likely fault location:
Fault localization scores:
[__blast_assert.assertion.1]: Single Bug Optimal Fault Localization
0.8: 39, 100, 103
0.6: 47, 65, 74
0.4: 57, 59, 61
0.2: 54

real	0m0.184s
user	0m0.140s
sys	0m0.036s

********************************************

SINGLE TRACE:
BUGS (# adjusted line numberS): 49, 75-6, 96, 103

time ~/cbmc/src/cbmc/cbmc rule57_ebda_blast.c_unsafe.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 1 --unwind-max 1

***after compute spectra***
**ef:
1 1 0 0 1 1 
**ep:
0 0 0 0 0 0 
**nf:
0 0 1 1 0 0 
**np:
0 0 0 0 0 0 

***after measure_sb***
1 1 0 0 1 1 


** Most likely fault location:
Fault localization scores:
[__blast_assert.assertion.1]: Single Bug Optimal Fault Localization
1: 39, 47, 54, 57, 59, 61, 65, 74, 100, 103

0: 49, 49, 82, 89, 92, 94, 96

real	0m0.169s
user	0m0.112s
sys	0m0.048s

