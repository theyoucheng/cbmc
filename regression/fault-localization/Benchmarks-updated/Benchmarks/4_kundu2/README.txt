SVCOMP 2013

BUGS: 15, 429-443, 613-614

SCORE: F = 1, P = 13, Sc = 7

time ~/cbmc/src/cbmc/cbmc kundu2_unsafe.cil.c --incremental --localize-faults --localize-faults-max-traces 20 --stop-on-fail --localize-faults-method sbo --verbosity 0 --beautify --unwind-min 2 --unwind-max 2

*** after simplification (blocks) *** 
[209] ##file kundu2_unsafe.cil.c line 345 function eval ##file kundu2_unsafe.cil.c line 543 function start_simulation ##file kundu2_unsafe.cil.c line 547 function start_simulation 
[239] ##file kundu2_unsafe.cil.c line 427 function fire_time_events ##file kundu2_unsafe.cil.c line 428 function fire_time_events ##file kundu2_unsafe.cil.c line 429 function fire_time_events ##file kundu2_unsafe.cil.c line 555 function start_simulation ##file kundu2_unsafe.cil.c line 557 function start_simulation 
[265] ##file kundu2_unsafe.cil.c line 439 function reset_time_events ##file kundu2_unsafe.cil.c line 444 function reset_time_events ##file kundu2_unsafe.cil.c line 449 function reset_time_events 
[278] ##file kundu2_unsafe.cil.c line 322 function exists_runnable_thread ##file kundu2_unsafe.cil.c line 508 function stop_simulation ##file kundu2_unsafe.cil.c line 566 function start_simulation 
[280] ##file kundu2_unsafe.cil.c line 510 function stop_simulation ##file kundu2_unsafe.cil.c line 511 function stop_simulation 
[10683] ##file kundu2_unsafe.cil.c line 359 function eval ##file kundu2_unsafe.cil.c line 361 function eval ##file kundu2_unsafe.cil.c line 49 function write_data ##file kundu2_unsafe.cil.c line 85 function P_1 ##file kundu2_unsafe.cil.c line 86 function P_1 ##file kundu2_unsafe.cil.c line 87 function P_1 ##file kundu2_unsafe.cil.c line 88 function P_1 
[10684] ##file kundu2_unsafe.cil.c line 143 function P_2 ##file kundu2_unsafe.cil.c line 144 function P_2 ##file kundu2_unsafe.cil.c line 156 function P_2 ##file kundu2_unsafe.cil.c line 157 function P_2 ##file kundu2_unsafe.cil.c line 374 function eval ##file kundu2_unsafe.cil.c line 376 function eval 
[10686] ##file kundu2_unsafe.cil.c line 51 function write_data ##file kundu2_unsafe.cil.c line 52 function write_data 
[10687] ##file kundu2_unsafe.cil.c line 389 function eval ##file kundu2_unsafe.cil.c line 391 function eval 
[10689] ##file kundu2_unsafe.cil.c line 221 function C_1 ##file kundu2_unsafe.cil.c line 226 function C_1 
[10690] ##file kundu2_unsafe.cil.c line 235 function C_1 ##file kundu2_unsafe.cil.c line 236 function C_1 ##file kundu2_unsafe.cil.c line 237 function C_1 ##file kundu2_unsafe.cil.c line 238 function C_1 ##file kundu2_unsafe.cil.c line 8 function error 
[10692] ##file kundu2_unsafe.cil.c line 30 function read_data ##file kundu2_unsafe.cil.c line 31 function read_data 
[10696] ##file kundu2_unsafe.cil.c line 326 function exists_runnable_thread ##file kundu2_unsafe.cil.c line 332 function exists_runnable_thread 
[10697] ##file kundu2_unsafe.cil.c line 82 function P_1 
[10699] ##file kundu2_unsafe.cil.c line 140 function P_2 
[10700] ##file kundu2_unsafe.cil.c line 256 function is_C_1_triggered ##file kundu2_unsafe.cil.c line 258 function is_C_1_triggered 
[10701] ##file kundu2_unsafe.cil.c line 482 function activate_threads ##file kundu2_unsafe.cil.c line 483 function activate_threads 
[10703] ##file kundu2_unsafe.cil.c line 266 function is_C_1_triggered ##file kundu2_unsafe.cil.c line 553 function start_simulation 
[10704] ##file kundu2_unsafe.cil.c line 106 function is_P_1_triggered ##file kundu2_unsafe.cil.c line 108 function is_P_1_triggered 
[10706] ##file kundu2_unsafe.cil.c line 466 function activate_threads ##file kundu2_unsafe.cil.c line 467 function activate_threads 
[10707] ##file kundu2_unsafe.cil.c line 174 function is_P_2_triggered ##file kundu2_unsafe.cil.c line 176 function is_P_2_triggered 
[10709] ##file kundu2_unsafe.cil.c line 474 function activate_threads ##file kundu2_unsafe.cil.c line 475 function activate_threads 
[10710] ##file kundu2_unsafe.cil.c line 268 function is_C_1_triggered 
[10712] ##file kundu2_unsafe.cil.c line 259 function is_C_1_triggered 
[true] ##file kundu2_unsafe.cil.c line 116 function is_P_1_triggered ##file kundu2_unsafe.cil.c line 146 function P_2 ##file kundu2_unsafe.cil.c line 148 function P_2 ##file kundu2_unsafe.cil.c line 149 function P_2 ##file kundu2_unsafe.cil.c line 151 function P_2 ##file kundu2_unsafe.cil.c line 184 function is_P_2_triggered ##file kundu2_unsafe.cil.c line 267 function is_C_1_triggered ##file kundu2_unsafe.cil.c line 276 function is_C_1_triggered ##file kundu2_unsafe.cil.c line 294 function init_threads ##file kundu2_unsafe.cil.c line 299 function init_threads ##file kundu2_unsafe.cil.c line 304 function init_threads ##file kundu2_unsafe.cil.c line 316 function exists_runnable_thread ##file kundu2_unsafe.cil.c line 317 function exists_runnable_thread ##file kundu2_unsafe.cil.c line 348 function eval ##file kundu2_unsafe.cil.c line 355 function eval ##file kundu2_unsafe.cil.c line 357 function eval ##file kundu2_unsafe.cil.c line 370 function eval ##file kundu2_unsafe.cil.c line 372 function eval ##file kundu2_unsafe.cil.c line 385 function eval ##file kundu2_unsafe.cil.c line 387 function eval ##file kundu2_unsafe.cil.c line 464 function activate_threads ##file kundu2_unsafe.cil.c line 472 function activate_threads ##file kundu2_unsafe.cil.c line 480 function activate_threads ##file kundu2_unsafe.cil.c line 528 function start_simulation ##file kundu2_unsafe.cil.c line 536 function start_simulation ##file kundu2_unsafe.cil.c line 539 function start_simulation ##file kundu2_unsafe.cil.c line 584 function init_model ##file kundu2_unsafe.cil.c line 585 function init_model ##file kundu2_unsafe.cil.c line 586 function init_model ##file kundu2_unsafe.cil.c line 597 function main ##file kundu2_unsafe.cil.c line 598 function main ##file kundu2_unsafe.cil.c line 599 function main ##file kundu2_unsafe.cil.c line 601 function main ##file kundu2_unsafe.cil.c line 602 function main ##file kundu2_unsafe.cil.c line 603 function main ##file kundu2_unsafe.cil.c line 604 function main ##file kundu2_unsafe.cil.c line 606 function main 

failing traces: 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 

passing traces: 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 
1 0 0 1 1 1 1 1 1 0 1 0 0 1 1 1 1 1 1 0 1 0 0 1 1 
1 0 0 1 1 1 1 1 0 0 1 0 0 1 1 0 0 1 0 0 0 0 0 0 1 
1 0 0 1 1 1 1 0 1 1 1 0 0 1 1 0 0 1 0 0 0 0 0 0 1 
1 0 0 1 1 1 0 0 1 0 1 0 0 1 0 0 0 1 0 0 0 0 0 0 1 
1 0 0 1 1 1 1 0 0 0 1 0 0 0 1 0 0 1 0 0 0 0 0 0 1 
1 0 0 1 1 1 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 
1 0 0 1 1 0 0 0 1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 1 
1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 
1 1 1 1 1 1 1 0 1 1 1 0 1 1 0 1 1 1 1 1 1 1 1 1 1 

***after compute spectra***
**ef:
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
**ep:
13 6 6 13 13 12 10 7 10 7 11 4 6 8 7 7 7 13 7 6 7 6 6 7 13 
**nf:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
**np:
0 7 7 0 0 1 3 6 3 6 2 9 7 5 6 6 6 0 6 7 6 7 7 6 0 

***after measure_sb***
0.0714286 0.571429 0.571429 0.0714286 0.0714286 0.142857 0.285714 0.5 0.285714 0.5 0.214286 0.714286 0.571429 0.428571 0.5 0.5 0.5 0.0714286 0.5 0.571429 0.5 0.571429 0.571429 0.5 0.0714286 


** Most likely fault location:
Fault localization scores:
[error.assertion.1]: Single Bug Optimal Fault Localization
0.714286: 30, 31
0.571429: 268, 326, 332, 427, 428, 429, 439, 444, 449, 466, 467, 474, 475, 555, 557
0.5: 51, 52, 106, 108, 140, 174, 176, 221, 226, 256, 258, 259, 482, 483
0.428571: 82
0.285714: 143, 144, 156, 157, 374, 376, 389, 391
0.214286: 8, 235, 236, 237, 238
0.142857: 49, 85, 86, 87, 88, 359, 361
0.0714286: 116, 146, 148, 149, 151, 184, 266, 267, 276, 294, 299, 304, 316, 317, 322, 345, 348, 355, 357, 370, 372, 385, 387, 464, 472, 480, 508, 510, 511, 528, 536, 539, 543, 547, 553, 566, 584, 585, 586, 597, 598, 599, 601, 602, 603, 604, 606

real	0m0.595s
user	0m0.556s
sys	0m0.032s


*****************************************************************************

SCORE: 

 time ~/cbmc/src/cbmc/cbmc kundu2_unsafe.cil.c --incremental --localize-faults --localize-faults-max-traces 0 --stop-on-fail --localize-faults-method sbo --single-trace --verbosity 0 --beautify --unwind-min 2 --unwind-max 2

***after compute spectra***
**ef:
0 0 1 1 1 1 1 0 0 0 1 1 0 1 1 0 1 1 0 1 0 0 0 1 1 0 1 1 1 0 1 1 0 1 1 0 1 1 0 1 1 
**ep:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
**nf:
1 1 0 0 0 0 0 1 1 1 0 0 1 0 0 1 0 0 1 0 1 1 1 0 0 1 0 0 0 1 0 0 1 0 0 1 0 0 1 0 0 
**np:
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

***after measure_sb***
0 0 1 1 1 1 1 0 0 0 1 1 0 1 1 0 1 1 0 1 0 0 0 1 1 0 1 1 1 0 1 1 0 1 1 0 1 1 0 1 1 


** Most likely fault location:
Fault localization scores:
[error.assertion.1]: Single Bug Optimal Fault Localization

1: 8, 30, 31, 49, 51, 52, 82, 85, 86, 87, 88, 106, 108, 116, 140, 143, 144, 146, 148, 149, 151, 156, 157, 174, 176, 184, 221, 226, 235, 236, 237, 238, 256, 258, 259, 266, 267, 268, 276, 294, 299, 304, 316, 317, 322, 326, 332, 345, 348, 355, 357, 359, 361, 370, 372, 374, 376, 385, 387, 389, 391, 427, 428, 429, 439, 444, 449, 464, 466, 467, 472, 474, 475, 480, 482, 483, 508, 510, 511, 528, 536, 539, 543, 547, 553, 555, 557, 566, 584, 585, 586, 597, 598, 599, 601, 602, 603, 604, 606

0: 26, 27, 39, 48, 91, 96, 107, 109, 159, 164, 175, 177, 215, 216, 217, 218, 219, 241, 246, 257, 269, 320, 321, 324, 325, 512, 516

real	0m0.583s
user	0m0.520s
sys	0m0.028s


