SV17

BUGS: 1930-1937, 2020-2032, 2077-2084, 2136-2138

best and worst and average = 1

time ../cbmc/src/cbmc/cbmc cdaudio_test.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --unwind-min 2 --unwind-max 2
