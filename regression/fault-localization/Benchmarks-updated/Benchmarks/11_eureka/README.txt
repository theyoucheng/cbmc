SVCOMP 2013

BUGS: 37

SCORE: Sc=3, F=1, P=7

time ~/cbmc/src/cbmc/cbmc eureka_01_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 3 --unwind-max 3


*** after simplification (blocks) *** 
[1] ##file eureka_01_unsafe.c line 20 function main ##file eureka_01_unsafe.c line 22 function main 
[4317] ##file eureka_01_unsafe.c line 21 function main ##file eureka_01_unsafe.c line 25 function main 
[4318] ##file eureka_01_unsafe.c line 35 function main ##file eureka_01_unsafe.c line 37 function main 
[4319] ##file eureka_01_unsafe.c line 51 function main ##file eureka_01_unsafe.c line 53 function main 
[true] ##file eureka_01_unsafe.c line 10 function main ##file eureka_01_unsafe.c line 11 function main 

failing traces: 
1 1 1 1 1 

passing traces: 
1 0 1 1 1 
1 0 0 1 1 
1 0 1 0 1 
0 0 0 0 1 
1 1 1 0 1 
1 1 1 1 1 
1 1 0 1 1 

***after compute spectra***
**ef:
1 1 1 1 1 
**ep:
6 3 4 4 7 
**nf:
0 0 0 0 0 
**np:
1 4 3 3 0 

***after measure_sb***
0.25 0.625 0.5 0.5 0.125 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
0.625: 21, 25
0.5: 35, 37, 51, 53
0.25: 20, 22
0.125: 10, 11

real	0m0.264s
user	0m0.240s
sys	0m0.016s

)))))))))))))))))))))))))))))))))))))))))))))

time ~/cbmc/src/cbmc/cbmc eureka_01_unsafe.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 3 --unwind-max 3

***after compute spectra***
**ef:
1 1 1 1 1 
**ep:
0 0 0 0 0 
**nf:
0 0 0 0 0 
**np:
0 0 0 0 0 

***after measure_sb***
1 1 1 1 1 


** Most likely fault location:
Fault localization scores:
[__VERIFIER_assert.assertion.1]: Single Bug Optimal Fault Localization
1: 10, 11, 20, 21, 22, 25, 35, 37, 51, 53

real	0m0.165s
user	0m0.112s
sys	0m0.036s

