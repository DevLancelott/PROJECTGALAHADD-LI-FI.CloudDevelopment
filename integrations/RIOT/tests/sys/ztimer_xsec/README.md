# Introduction

This application tests the high abstraction level ztimer clocks usec, msec and sec
by locking three mutexes and waiting for them to
be unlocked by ZTIMER*USEC, ZTIMER*MSEC and ZTIMER_SEC
The tests succeeds if the board running the test does not get stuck.

ZTIMER*MSEC and ZTIMER*SEC will be configured following the rules described
in the ztimer documentation (one may want to use extra ztimer*perih**).
Timing information is provided for human analysis it is not checked by automatic
testing, since they are system and runtime dependent, there are other tests
that (partially) cover the accuracy of timers.
