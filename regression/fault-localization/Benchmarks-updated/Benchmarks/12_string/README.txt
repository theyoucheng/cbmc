SVCOMP 2013

BUGS: 53

SCORE = Sc= 2, F=3, P=3

time ~/cbmc/src/cbmc/cbmc string_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 6 --unwind-max 6

*** after simplification (blocks) *** 
[3147] ##file string_unsafe.c line 41 function main ##file string_unsafe.c line 44 function main 
[3149] ##file string_unsafe.c line 43 function main 
[true] ##file string_unsafe.c line 20 function main ##file string_unsafe.c line 24 function main ##file string_unsafe.c line 53 function main ##file string_unsafe.c line 55 function main 

failing traces: 
1 1 1 
1 0 1 
0 0 1 

passing traces: 
0 0 1 
1 0 1 
1 1 1 

***after compute spectra***
**ef:
2 1 3 
**ep:
2 1 3 
**nf:
1 2 0 
**np:
1 2 0 

***after measure_sb***
1.5 0.75 2.25 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
2.25: 20, 24, 53, 55
1.5: 41, 44
0.75: 43

real	0m0.167s
user	0m0.132s
sys	0m0.024s


_____________________________________________

 time ~/cbmc/src/cbmc/cbmc string_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 6 --unwind-max 6

***after compute spectra***
**ef:
1 0 1 1 
**ep:
0 0 0 0 
**nf:
0 1 0 0 
**np:
0 0 0 0 

***after measure_sb***
1 0 1 1 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
1: 20, 24, 41, 43, 44, 53, 55
0: 48

real	0m0.166s
user	0m0.140s
sys	0m0.012s


