SVCOMP13

Bug: 73

SCORE: Sc=0, F=3

time ~/cbmc/src/cbmc/cbmc byte_add_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 3 --unwind-max 3

*** after simplification (blocks) *** 
[714] ##file byte_add_unsafe.c line 71 function mp_add ##file byte_add_unsafe.c line 78 function mp_add 
[715] ##file byte_add_unsafe.c line 61 function mp_add ##file byte_add_unsafe.c line 62 function mp_add 
[716] ##file byte_add_unsafe.c line 67 function mp_add ##file byte_add_unsafe.c line 68 function mp_add 
[717] ##file byte_add_unsafe.c line 73 function mp_add ##file byte_add_unsafe.c line 74 function mp_add 
[718] ##file byte_add_unsafe.c line 57 function mp_add ##file byte_add_unsafe.c line 58 function mp_add 
[719] ##file byte_add_unsafe.c line 63 function mp_add 
[720] ##file byte_add_unsafe.c line 79 function mp_add 
[721] ##file byte_add_unsafe.c line 69 function mp_add 
[722] ##file byte_add_unsafe.c line 90 function mp_add ##file byte_add_unsafe.c line 92 function mp_add 
[true] ##file byte_add_unsafe.c line 105 function main ##file byte_add_unsafe.c line 107 function main ##file byte_add_unsafe.c line 23 function mp_add ##file byte_add_unsafe.c line 24 function mp_add ##file byte_add_unsafe.c line 25 function mp_add ##file byte_add_unsafe.c line 26 function mp_add ##file byte_add_unsafe.c line 28 function mp_add ##file byte_add_unsafe.c line 29 function mp_add ##file byte_add_unsafe.c line 30 function mp_add ##file byte_add_unsafe.c line 31 function mp_add ##file byte_add_unsafe.c line 95 function mp_add 

failing traces: 
1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 0 1 1 1 1 
1 1 1 1 1 0 1 0 1 1 

passing traces: 
1 1 1 0 1 1 1 1 1 1 
1 1 1 0 1 0 1 1 1 1 
1 1 1 1 1 0 1 0 1 1 
1 1 1 0 0 0 0 0 1 1 
1 1 1 1 1 1 1 0 1 1 
1 1 1 0 1 1 1 0 1 1 
1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 0 1 1 1 1 

***after compute spectra***
**ef:
3 3 3 3 3 1 3 2 3 3 
**ep:
8 8 8 4 7 4 7 4 8 8 
**nf:
0 0 0 0 0 2 0 1 0 0 
**np:
0 0 0 4 1 4 1 4 0 0 

***after measure_sb***
2.11111 2.11111 2.11111 2.55556 2.22222 0.555556 2.22222 1.55556 2.11111 2.11111 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
2.55556: 73, 74
2.22222: 57, 58, 79
2.11111: 23, 24, 25, 26, 28, 29, 30, 31, 61, 62, 67, 68, 71, 78, 90, 92, 95, 105, 107
1.55556: 69
0.555556: 63

real	0m0.180s
user	0m0.156s
sys	0m0.012s

#####################################

time ~/cbmc/src/cbmc/cbmc byte_add_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 3 --unwind-max 3

***after compute spectra***
**ef:
0 0 0 1 1 1 1 1 1 1 1 1 1 
**ep:
0 0 0 0 0 0 0 0 0 0 0 0 0 
**nf:
1 1 1 0 0 0 0 0 0 0 0 0 0 
**np:
0 0 0 0 0 0 0 0 0 0 0 0 0 

***after measure_sb***
0 0 0 1 1 1 1 1 1 1 1 1 1 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
1: 23, 24, 25, 26, 28, 29, 30, 31, 57, 58, 61, 62, 63, 67, 68, 69, 71, 73, 74, 78, 79, 90, 92, 95, 105, 107

0: 64, 70, 80

real	0m0.177s
user	0m0.128s
sys	0m0.036s

