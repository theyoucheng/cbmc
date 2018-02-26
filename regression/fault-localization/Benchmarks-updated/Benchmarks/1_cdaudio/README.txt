SV17

BUGS: 1930-1937, 2020-2032, 2077-2084, 2136-2138

SCORE: (B = 13, W = 14, f = 1), Sc=, 13, F = 3, P = 8

time ~/cbmc/src/cbmc/cbmc cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --slice-formula --beautify --verbosity 0 --unwind-min 2 --unwind-max 2

*** after simplification (blocks) *** 
[6] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1775 function main ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1779 function main 
[8] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1793 function main ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 312 function CdAudioDeviceControl 
[26] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1590 function CdAudioHPCdrDeviceControl ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 344 function CdAudioDeviceControl 
[463] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1568 function HPCdrCompletion ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1614 function CdAudioHPCdrDeviceControl 
[466] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1561 function HpCdrProcessLastSession ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1625 function CdAudioHPCdrDeviceControl ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1909 function IofCallDriver ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1919 function IofCallDriver 
[468] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1558 function HpCdrProcessLastSession ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1581 function HPCdrCompletion 
[483] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1957 function IofCallDriver ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1959 function IofCallDriver 
[507] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1632 function CdAudioHPCdrDeviceControl ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 347 function CdAudioDeviceControl 
[530] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1797 function main 
[6684] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1899 function IofCallDriver ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 373 function CdAudioSendToNextDriver 
[6691] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1928 function IofCallDriver ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1931 function IofCallDriver 
[6692] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1933 function IofCallDriver ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1936 function IofCallDriver 
[6698] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 353 function CdAudioDeviceControl 
[true] ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1719 function main ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1721 function main ##file cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c line 1722 function main 

failing traces: 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 0 1 1 1 1 1 1 1 1 
1 1 1 1 1 0 1 1 1 1 1 0 1 1 

passing traces: 
1 1 0 0 0 0 0 0 1 1 1 1 1 1 
1 1 0 0 0 0 0 0 1 1 0 0 1 1 
1 1 1 1 1 0 1 1 1 1 0 0 1 1 
1 1 0 0 0 0 0 0 1 0 0 0 1 1 
1 0 0 0 0 0 0 0 0 1 1 0 0 1 
0 0 0 0 0 0 0 0 0 0 0 0 0 1 
1 1 1 0 0 0 0 1 1 1 1 0 1 1 
1 1 1 1 1 1 1 1 1 1 1 0 1 1 
1 1 1 0 0 0 0 1 1 1 1 1 1 1 
1 1 1 0 0 0 0 1 1 1 0 0 1 1 
1 1 0 0 0 0 0 0 1 1 1 0 1 1 
1 1 1 1 1 1 1 1 1 1 0 0 1 1 
1 1 1 1 1 0 1 1 1 1 1 0 1 1 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 0 1 1 1 1 1 1 1 1 

***after compute spectra***
**ef:
3 3 3 3 3 1 3 3 3 3 3 2 3 3 
**ep:
14 13 9 6 6 3 6 9 13 13 9 4 13 15 
**nf:
0 0 0 0 0 2 0 0 0 0 0 1 0 0 
**np:
1 2 6 9 9 12 9 6 2 2 6 11 2 0 

***after measure_sb***
2.125 2.1875 2.4375 2.625 2.625 0.8125 2.625 2.4375 2.1875 2.1875 2.4375 1.75 2.1875 2.0625 


** Most likely fault location:
Fault localization scores:
[errorFn.assertion.1]: Single Bug Optimal Fault Localization
2.625: 1561, 1568, 1614, 1625, 1909, 1919, 1957, 1959
2.4375: 344, 347, 1590, 1632, 1928, 1931
2.1875: 312, 353, 373, 1793, 1797, 1899
2.125: 1775, 1779
2.0625: 1719, 1721, 1722
1.75: 1933, 1936
0.8125: 1558, 1581

real	0m1.095s
user	0m1.036s
sys	0m0.044s

_____________________________________

SCORES Without passing trace:
1561, 1568, 1614, 1625, 1909, 1919, 344, 347, 1590, 1632, 1928, 312, 353, 373, 1793, 1797, 1899, 1775, 1779, 1719, 1721, 1722, 1931
1957, 1959,

*************************

BUGS: 1930-1937, 2020-2032, 2077-2084, 2136-2138

SCORE: 24

SINGLE TRACE

time ~/cbmc/src/cbmc/cbmc cdaudio_simpl1_false-unreach-call_true-valid-memsafety_true-termination.cil.c --incremental --localize-faults --localize-faults-max-traces 0 --stop-on-fail --localize-faults-method sbo --slice-formula --beautify --single-trace --verbosity 0 --unwind-min 2 --unwind-max 2

***after compute spectra***
**ef:
1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 0 0 1 1 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1 0 0 0 0 0 1 0 1 
**ep:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
**nf:
0 0 1 1 1 1 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 1 1 0 0 1 1 1 1 1 1 1 1 1 0 1 1 1 1 1 1 0 0 1 1 1 1 1 0 1 0 
**np:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

***after measure_sb***
1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 1 1 0 0 1 1 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 1 0 0 0 0 0 1 0 1 


** Most likely fault location:
Fault localization scores:
[errorFn.assertion.1]: Single Bug Optimal Fault Localization
1: 312, 344, 347, 353, 373, 1558, 1561, 1568, 1581, 1590, 1614, 1625, 1632, 1719, 1721, 1722, 1775, 1779, 1793, 1797, 1899, 1909, 1919, 1928, 1931, 1933, 1936, 1957, 1959

real	0m1.040s
user	0m0.964s
sys	0m0.068s



