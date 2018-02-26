SVCOMP 2017

BUG: 118

time ~/cbmc/src/cbmc/cbmc merge_sort_false-unreach-call_false-valid-memcleanup.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 2 --unwind-max 2

*** after simplification (blocks) *** 
[-4] ##file merge_sort_false-unreach-call_false-valid-memcleanup.c line 131 function main ##file merge_sort_false-unreach-call_false-valid-memcleanup.c line 133 function main 
[264740] ##file merge_sort_false-unreach-call_false-valid-memcleanup.c line 115 function main ##file merge_sort_false-unreach-call_false-valid-memcleanup.c line 118 function main ##file merge_sort_false-unreach-call_false-valid-memcleanup.c line 119 function main ##file merge_sort_false-unreach-call_false-valid-memcleanup.c line 125 function main ##file merge_sort_false-unreach-call_false-valid-memcleanup.c line 126 function main 

failing traces: 
1 1 

passing traces: 
1 1 
0 0 

***after compute spectra***
**ef:
1 1 
**ep:
1 1 
**nf:
0 0 
**np:
1 1 

***after measure_sb***
0.666667 0.666667 


** Most likely fault location:
Fault localization scores:
[fail.assertion.1]: Single Bug Optimal Fault Localization
0.666667: 115, 118, 119, 125, 126, 131, 133

real	0m2.322s
user	0m2.228s
sys	0m0.084s

*******************************************

time ~/cbmc/src/cbmc/cbmc merge_sort_false-unreach-call_false-valid-memcleanup.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 2 --unwind-max 2

***after compute spectra***
**ef:
1 1 0 
**ep:
0 0 0 
**nf:
0 0 1 
**np:
0 0 0 

***after measure_sb***
1 1 0 


** Most likely fault location:
Fault localization scores:
[fail.assertion.1]: Single Bug Optimal Fault Localization
1: 115, 118, 119, 125, 126, 131, 133
0: 8, 83

real	0m2.190s
user	0m2.080s
sys	0m0.092s



