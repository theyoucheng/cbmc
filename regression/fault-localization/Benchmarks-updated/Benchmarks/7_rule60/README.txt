SVCOMP 2013

BUG (# adjusted): 59

SCORE: Sc=8, F=1, P=3

time ~/cbmc/src/cbmc/cbmc rule60_list2.c_unsafe_1.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 1 --unwind-max 1


*** after simplification (blocks) *** 
[591] ##file files/rule60_list2.c line 36 function list_add ##file files/rule60_list2.c line 37 function list_add ##file files/rule60_list2.c line 42 function list_del ##file files/rule60_list2.c line 53 function main ##file files/rule60_list2.c line 54 function main ##file files/rule60_list2.c line 55 function main ##file files/rule60_list2.c line 56 function main ##file files/rule60_list2.c line 59 function main 
[592] ##file files/rule60_list2.c line 38 function list_add 
[true] ##file files/rule60_list2.c line 17 function __getMemory ##file files/rule60_list2.c line 19 function __getMemory ##file files/rule60_list2.c line 25 function my_malloc ##file files/rule60_list2.c line 50 function main ##file files/rule60_list2.c line 51 function main ##file files/rule60_list2.c line 52 function main 

failing traces: 
1 1 1 

passing traces: 
1 1 1 
1 0 1 
0 0 1 

***after compute spectra***
**ef:
1 1 1 
**ep:
2 1 3 
**nf:
0 0 0 
**np:
1 2 0 

***after measure_sb***
0.5 0.75 0.25 


** Most likely fault location:
Fault localization scores:
[__blast_assert.assertion.1]: Single Bug Optimal Fault Localization
0.75: 38
0.5: 36, 37, 42, 53, 54, 55, 56, 59
0.25: 17, 19, 25, 50, 51, 52

real	0m0.180s
user	0m0.072s
sys	0m0.100s

****************************************************

ST
SC = 14

time ~/cbmc/src/cbmc/cbmc rule60_list2.c_unsafe_1.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 1 --unwind-max 1

***after compute spectra***
**ef:
1 1 1 
**ep:
0 0 0 
**nf:
0 0 0 
**np:
0 0 0 

***after measure_sb***
1 1 1 


** Most likely fault location:
Fault localization scores:
[__blast_assert.assertion.1]: Single Bug Optimal Fault Localization
1: 17, 19, 25, 36, 37, 38, 42, 50, 51, 52, 53, 54, 55, 56, 59

real	0m0.173s
user	0m0.116s
sys	0m0.028s

