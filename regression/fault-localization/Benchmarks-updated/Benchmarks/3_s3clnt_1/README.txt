SVCOMP 2017

BUG: 312

SCORE: Sc=3, F = 2, P = 7

time ~/cbmc/src/cbmc/cbmc s3_clnt_1_false-unreach-call.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --slice-formula --beautify --unwind-min 6 --unwind-max 6

*** after simplification (blocks) *** 
[1] ##file s3_clnt_1_false-unreach-call.cil.c line 84 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 85 function ssl3_connect 
[10925] ##file s3_clnt_1_false-unreach-call.cil.c line 117 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 249 function ssl3_connect 
[10926] ##file s3_clnt_1_false-unreach-call.cil.c line 123 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 264 function ssl3_connect 
[10928] ##file s3_clnt_1_false-unreach-call.cil.c line 100 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 98 function ssl3_connect 
[10932] ##file s3_clnt_1_false-unreach-call.cil.c line 281 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 283 function ssl3_connect 
[10933] ##file s3_clnt_1_false-unreach-call.cil.c line 293 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 296 function ssl3_connect 
[10934] ##file s3_clnt_1_false-unreach-call.cil.c line 309 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 312 function ssl3_connect 
[true] ##file s3_clnt_1_false-unreach-call.cil.c line 12 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 17 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 20 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 21 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 211 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 215 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 22 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 25 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 27 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 35 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 36 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 37 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 39 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 54 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 55 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 56 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 57 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 58 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 59 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 60 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 61 function ssl3_connect ##file s3_clnt_1_false-unreach-call.cil.c line 62 function ssl3_connect 

failing traces: 
1 1 1 1 1 1 1 1 
0 1 1 1 1 1 1 1 

passing traces: 
0 1 1 1 1 0 0 1 
0 1 1 1 0 0 0 1 
0 1 0 0 0 0 0 1 
0 0 0 0 0 0 0 1 
0 1 1 1 1 1 0 1 
0 1 1 1 0 1 1 1 
1 1 1 1 0 1 1 1 

***after compute spectra***
**ef:
1 2 2 2 2 2 2 2 
**ep:
1 6 5 5 2 3 2 7 
**nf:
1 0 0 0 0 0 0 0 
**np:
6 1 2 2 5 4 5 0 

***after measure_sb***
0.875 1.25 1.375 1.375 1.75 1.625 1.75 1.125 


** Most likely fault location:
Fault localization scores:
[ssl3_connect.assertion.1]: Single Bug Optimal Fault Localization
1.75: 281, 283, 309, 312
1.625: 293, 296
1.375: 98, 100, 123, 264
1.25: 117, 249
1.125: 12, 17, 20, 21, 22, 25, 27, 35, 36, 37, 39, 54, 55, 56, 57, 58, 59, 60, 61, 62, 211, 215
0.875: 84, 85

real	0m3.560s
user	0m3.484s
sys	0m0.040s

************************************************

SINGLE TRACE

time ~/cbmc/src/cbmc/cbmc s3_clnt_1_false-unreach-call.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --slice-formula --beautify --unwind-min 6 --unwind-max 6

***after compute spectra***
**ef:
1 0 0 0 0 1 1 0 1 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
**ep:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
**nf:
0 1 1 1 1 0 0 1 0 1 1 1 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 0 
**np:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

***after measure_sb***
1 0 0 0 0 1 1 0 1 0 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1 


** Most likely fault location:
Fault localization scores:
[ssl3_connect.assertion.1]: Single Bug Optimal Fault Localization
1: 12, 17, 20, 21, 22, 25, 27, 35, 36, 37, 39, 54, 55, 56, 57, 58, 59, 60, 61, 62, 84, 85, 98, 100, 117, 123, 211, 215, 249, 264, 281, 283, 293, 296, 309, 312

0: 87, 88, 222, 223, 230, 232, 245, 261, 321, 324, 337, 342, 348, 351, 363, 366, 373, 376, 396, 399, 401, 404, 405, 406, 407, 410, 411, 413, 414, 415, 423, 426, 436, 439, 440, 442, 444, 447, 453, 456, 460, 461, 518, 519, 525, 526, 528

real	0m3.515s
user	0m3.428s
sys	0m0.068s


