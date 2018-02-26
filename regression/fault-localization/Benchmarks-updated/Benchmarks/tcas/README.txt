BUG: 367

time ~/cbmc/src/cbmc/cbmc tcas.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 1 --unwind-max 1


SCORE: Sc = 5, F = 2, P = 4

time ~/cbmc/src/cbmc/cbmc tcas.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 1 --unwind-max 1

*** after simplification (blocks) *** 
[1142] ##file tcas.c line 173 function Non_Crossing_Biased_Climb 
[1144] ##file tcas.c line 116 function ALIM 
[1145] ##file tcas.c line 220 function Non_Crossing_Biased_Descend 
[true] ##file tcas.c line 160 function Non_Crossing_Biased_Climb ##file tcas.c line 213 function Non_Crossing_Biased_Descend ##file tcas.c line 363 function alt_sep_test ##file tcas.c line 367 function alt_sep_test ##file tcas.c line 429 function main ##file tcas.c line 431 function main ##file tcas.c line 432 function main ##file tcas.c line 437 function main ##file tcas.c line 438 function main 

failing traces: 
1 1 1 1 
1 0 1 1 

passing traces: 
1 0 1 1 
0 1 0 1 
1 1 1 1 
0 0 0 1 

***after compute spectra***
**ef:
2 1 2 2 
**ep:
2 2 2 4 
**nf:
0 1 0 0 
**np:
2 2 2 0 

***after measure_sb***
1.6 0.6 1.6 1.2 


** Most likely fault location:
Fault localization scores:
[alt_sep_test.assertion.5]: Single Bug Optimal Fault Localization
1.6: 173, 220
1.2: 160, 213, 363, 367, 429, 431, 432, 437, 438
0.6: 116

real	0m0.206s
user	0m0.160s
sys	0m0.036s

_________________

time ~/cbmc/src/cbmc/cbmc tcas.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 1 --unwind-max 1

SCORE = Sc = 6, F = 1

***after compute spectra***
**ef:
1 0 1 1 0 1 
**ep:
0 0 0 0 0 0 
**nf:
0 1 0 0 1 0 
**np:
0 0 0 0 0 0 

***after measure_sb***
1 0 1 1 0 1 


** Most likely fault location:
Fault localization scores:
[alt_sep_test.assertion.5]: Single Bug Optimal Fault Localization
1: 116, 160, 173, 213, 220, 363, 367, 429, 431, 432, 437, 438
0: 188, 233

real	0m0.196s
user	0m0.148s
sys	0m0.040s

