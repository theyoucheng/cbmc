SVCOMP 2013

BUG: 16

time ~/cbmc/src/cbmc/cbmc insertion_sort_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 4 --unwind-max 4

*** after simplification (blocks) *** 
[116269] ##file insertion_sort_unsafe.c line 13 function main ##file insertion_sort_unsafe.c line 14 function main ##file insertion_sort_unsafe.c line 21 function main 
[116270] ##file insertion_sort_unsafe.c line 16 function main ##file insertion_sort_unsafe.c line 17 function main 
[116271] ##file insertion_sort_unsafe.c line 23 function main ##file insertion_sort_unsafe.c line 24 function main 
[true] ##file insertion_sort_unsafe.c line 10 function main 

failing traces: 
1 1 1 1 

passing traces: 
0 0 0 1 
1 1 1 1 
1 0 1 1 

***after compute spectra***
**ef:
1 1 1 1 
**ep:
2 1 2 3 
**nf:
0 0 0 0 
**np:
1 2 1 0 

***after measure_sb***
0.5 0.75 0.5 0.25 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
0.75: 16, 17
0.5: 13, 14, 21, 23, 24
0.25: 10

real	0m4.280s
user	0m4.208s
sys	0m0.052s

__________________________________________

time ~/cbmc/src/cbmc/cbmc insertion_sort_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 4 --unwind-max 4

***after compute spectra***
**ef:
1 1 1 1 
**ep:
0 0 0 0 
**nf:
0 0 0 0 
**np:
0 0 0 0 

***after measure_sb***
1 1 1 1 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
1: 10, 13, 14, 16, 17, 21, 23, 24

real	0m1.046s
user	0m0.992s
sys	0m0.040s

_____________________________________________

time ~/cbmc/src/cbmc/cbmc insertion_sort_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --unwind-min 4 --unwind-max 4

***after compute spectra***
**ef:
1 1 
**ep:
0 0 
**nf:
0 0 
**np:
0 0 

***after measure_sb***
1 1 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
1: 10, 13, 14, 15, 16, 17, 19, 21, 23, 24

real	0m1.915s
user	0m1.780s
sys	0m0.124s


