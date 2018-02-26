SV2017

BUGS: 956-958, 961-963, 1055-1057, 1077-1079, 1099-1101, 1120-1122

SCORE: (B = 1, W = 5, f = 1,), Sc= 8, F = 2, P = 8

time ~/cbmc/src/cbmc/cbmc floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --slice-formula --beautify --verbosity 0 --unwind-min 1 --unwind-max 1

*** after simplification (blocks) *** 
[2] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 168 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 169 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 172 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 173 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 175 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 177 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 735 function main 
[134] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 102 function FlQueueIrpToThread ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 103 function FlQueueIrpToThread ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 254 function FloppyPnp 
[137] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 117 function FlQueueIrpToThread ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 119 function FlQueueIrpToThread 
[139] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1100 function PsCreateSystemThread ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 120 function FlQueueIrpToThread 
[143] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1109 function PsCreateSystemThread ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 127 function FlQueueIrpToThread 
[146] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1078 function ObReferenceObjectByHandle ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 137 function FlQueueIrpToThread 
[150] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1087 function ObReferenceObjectByHandle ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1121 function ZwClose ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 141 function FlQueueIrpToThread 
[165] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 153 function FlQueueIrpToThread ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 258 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 261 function FloppyPnp 
[167] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1048 function KeWaitForSingleObject 
[171] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 283 function FloppyPnp 
[260] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 429 function FloppyPnp 
[266] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 430 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 739 function main 
[2394] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 230 function FloppyPnp ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 442 function FloppyStartDevice ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 457 function FloppyStartDevice ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 458 function FloppyStartDevice ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 475 function FloppyStartDevice ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 957 function IofCallDriver 
[2395] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 943 function IofCallDriver 
[2396] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 959 function IofCallDriver ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 962 function IofCallDriver 
[2400] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1056 function KeWaitForSingleObject ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 64 function errorFn 
[2401] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 995 function IofCallDriver ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 997 function IofCallDriver 
[2404] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 417 function FloppyPnp 
[true] ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 674 function main ##file floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 731 function main 

failing traces: 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
1 1 1 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 
1 1 1 0 0 0 0 1 1 1 1 1 1 1 0 1 1 1 1 

passing traces: 
1 1 1 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 1 1 1 0 0 1 1 0 0 0 0 0 1 1 
1 1 1 1 1 0 0 1 0 0 1 1 0 0 0 0 0 1 1 
1 1 0 0 0 0 0 1 0 0 1 1 0 0 0 0 0 1 1 
1 0 0 0 0 0 0 0 0 0 0 1 1 1 0 0 1 0 1 
1 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 
1 0 0 0 0 0 0 0 0 0 1 1 1 1 0 0 1 1 1 
1 0 0 0 0 0 0 0 0 0 1 1 1 0 0 1 0 1 1 
1 0 0 0 0 0 0 0 0 0 1 1 1 0 1 1 0 1 1 

***after compute spectra***
**ef:
3 3 3 1 1 1 1 3 3 3 3 3 3 3 2 3 3 3 3 
**ep:
10 5 4 3 3 2 2 5 2 2 8 10 6 4 3 4 4 8 11 
**nf:
0 0 0 2 2 2 2 0 0 0 0 0 0 0 1 0 0 0 0 
**np:
1 6 7 8 8 9 9 6 9 9 3 1 5 7 8 7 7 3 0 

***after measure_sb***
2.16667 2.58333 2.66667 0.75 0.75 0.833333 0.833333 2.58333 2.83333 2.83333 2.33333 2.16667 2.5 2.66667 1.75 2.66667 2.66667 2.33333 2.08333 


** Most likely fault location:
Fault localization scores:
[errorFn.assertion.1]: Single Bug Optimal Fault Localization
2.83333: 283, 1048
2.66667: 64, 117, 119, 943, 995, 997, 1056
2.58333: 102, 103, 153, 254, 258, 261
2.5: 230, 442, 457, 458, 475, 957
2.33333: 417, 429
2.16667: 168, 169, 172, 173, 175, 177, 430, 735, 739
2.08333: 674, 731
1.75: 959, 962
0.833333: 137, 141, 1078, 1087, 1121
0.75: 120, 127, 1100, 1109

real	0m0.375s
user	0m0.340s
sys	0m0.020s

sys	0m0.028s

*****************************************************
NP

283, 1048, 64, 117, 119, 943, 995, 997, 102, 103, 153, 254, 258, 261, 230, 442, 457, 458, 475, 957, 417, 429, 168, 169, 172, 173, 175, 177, 430, 735, 739, 674, 731, 1056

____________________

SINGLE TRACE:

BUGS: 956-958, 961-963, 1055-1057, 1077-1079, 1099-1101, 1120-1122
time ~/cbmc/src/cbmc/cbmc floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --slice-formula --beautify --verbosity 0 --unwind-min 1 --unwind-max 1

***after compute spectra***
**ef:
1 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 1 1 1 1 1 0 0 0 1 1 0 0 1 1 
**ep:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
**nf:
0 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 0 0 0 0 0 1 1 1 0 0 1 1 0 0 
**np:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

***after measure_sb***
1 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 1 1 1 1 1 0 0 0 1 1 0 0 1 1 


** Most likely fault location:
Fault localization scores:
[errorFn.assertion.1]: Single Bug Optimal Fault Localization
1: 64, 102, 103, 117, 119, 120, 127, 137, 141, 153, 168, 169, 172, 173, 175, 177, 230, 254, 258, 261, 283, 417, 429, 430, 442, 457, 458, 475, 674, 731, 735, 739, 943, 957, 959, 962, 995, 997, 1048, 1056, 1078, 1087, 1100, 1109, 1121

0: 234, 252, 313, 338, 341, 372, 391, 393, 399, 510, 513, 528, 575, 583, 586, 626, 635, 638, 642, 813, 818, 839, 884, 905, 926, 982, 984, 986, 987, 989, 992, 1046

real	0m0.355s
user	0m0.296s
sys	0m0.040s

******************************************************

time ~/cbmc/src/cbmc/cbmc floppy_simpl3_false-unreach-call_true-valid-memsafety_true-termination.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --unwind-min 1 --unwind-max 1
***after compute spectra***
**ef:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 0 0 0 0 0 0 1 1 0 0 1 0 1 0 0 1 0 1 1 0 0 0 0 0 0 1 0 1 0 0 0 0 1 0 0 1 1 0 0 0 1 
**ep:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
**nf:
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 0 0 1 1 1 1 1 1 0 0 1 1 0 1 0 1 1 0 1 0 0 1 1 1 1 1 1 0 1 0 1 1 1 1 0 1 1 0 0 1 1 1 0 
**np:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

***after measure_sb***
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 0 0 0 0 0 0 1 1 0 0 1 0 1 0 0 1 0 1 1 0 0 0 0 0 0 1 0 1 0 0 0 0 1 0 0 1 1 0 0 0 1 


** Most likely fault location:
Fault localization scores:
[errorFn.assertion.1]: Single Bug Optimal Fault Localization
1: 64, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 101, 102, 103, 104, 108, 117, 119, 147, 153, 163, 164, 167, 168, 169, 170, 172, 173, 174, 175, 176, 177, 178, 182, 193, 194, 198, 199, 230, 234, 254, 257, 258, 261, 262, 264, 267, 270, 271, 272, 274, 281, 282, 283, 405, 408, 415, 416, 417, 429, 430, 437, 438, 440, 442, 446, 447, 452, 453, 456, 457, 458, 459, 461, 462, 463, 475, 486, 487, 488, 489, 490, 491, 492, 503, 507, 508, 509, 510, 517, 609, 660, 661, 662, 663, 664, 665, 671, 674, 675, 678, 679, 680, 681, 682, 683, 684, 685, 686, 687, 688, 689, 690, 691, 692, 693, 694, 695, 696, 697, 698, 699, 700, 701, 705, 706, 710, 711, 712, 713, 714, 715, 716, 717, 731, 735, 739, 926, 939, 945, 948, 957, 959, 962, 970, 972, 995, 996, 997, 1023, 1026, 1056
0: 113, 114, 115, 116, 120, 121, 122, 127, 131, 132, 133, 137, 141, 143, 200, 202, 203, 204, 205, 241, 243, 250, 251, 252, 287, 288, 289, 290, 291, 300, 301, 302, 304, 311, 312, 313, 316, 317, 318, 319, 320, 331, 335, 336, 337, 338, 341, 342, 344, 345, 350, 351, 352, 353, 354, 357, 359, 360, 361, 363, 370, 371, 372, 374, 376, 377, 378, 380, 387, 388, 389, 390, 391, 393, 395, 399, 401, 404, 513, 514, 516, 522, 523, 524, 525, 526, 527, 528, 530, 531, 533, 535, 546, 550, 573, 574, 575, 578, 583, 586, 588, 592, 593, 597, 598, 599, 618, 619, 620, 626, 630, 633, 634, 635, 638, 639, 641, 642, 718, 719, 720, 750, 751, 758, 759, 813, 816, 818, 839, 884, 905, 912, 914, 925, 966, 969, 973, 975, 982, 983, 984, 986, 987, 988, 989, 991, 992, 1012, 1013, 1035, 1037, 1038, 1045, 1046, 1078, 1087, 1100, 1109, 1121

real	0m0.406s
user	0m0.364s
sys	0m0.036s


