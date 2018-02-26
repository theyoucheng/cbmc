SVCOMP 2017

BUGS: 31, 34

time ~/cbmc/src/cbmc/cbmc alternating_list_false-unreach-call_false-valid-memcleanup.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 2 --unwind-max 2

time ~/cbmc/src/cbmc/cbmc alternating_list_false-unreach-call_false-valid-memcleanup.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 2 --unwind-max 2

*** after simplification (blocks) *** 
[70] ##file alternating_list_false-unreach-call_false-valid-memcleanup.c line 38 function main 
[2726] ##file alternating_list_false-unreach-call_false-valid-memcleanup.c line 29 function main ##file alternating_list_false-unreach-call_false-valid-memcleanup.c line 31 function main ##file alternating_list_false-unreach-call_false-valid-memcleanup.c line 39 function main ##file alternating_list_false-unreach-call_false-valid-memcleanup.c line 40 function main 
[2727] ##file alternating_list_false-unreach-call_false-valid-memcleanup.c line 42 function main 

failing traces: 
1 1 1 

passing traces: 
0 1 1 

***after compute spectra***
**ef:
1 1 1 
**ep:
0 1 1 
**nf:
0 0 0 
**np:
1 0 0 

***after measure_sb***
1 0.5 0.5 


** Most likely fault location:
Fault localization scores:
[main.assertion.9]: Single Bug Optimal Fault Localization
1: 38
0.5: 29, 31, 39, 40, 42

real	0m0.322s
user	0m0.256s
sys	0m0.036s


*********************************

time ~/cbmc/src/cbmc/cbmc alternating_list_false-unreach-call_false-valid-memcleanup.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 2 --unwind-max 2

***after compute spectra***
**ef:
0 1 0 0 1 1 0 
**ep:
0 0 0 0 0 0 0 
**nf:
1 0 1 1 0 0 1 
**np:
0 0 0 0 0 0 0 

***after measure_sb***
0 1 0 0 1 1 0 


** Most likely fault location:
Fault localization scores:
[main.assertion.9]: Single Bug Optimal Fault Localization
1: 29, 31, 38, 39, 40, 42
0: 30, 34, 51, 57, 62, 63, 64

real	0m0.314s
user	0m0.240s
sys	0m0.052s

