Expected result
===============
This test will blink all available on-board LEDs, one after each other in an
endless loop. Each LED will light up once long, and twice short, where the long
interval is roughly 4 times as long as the short interval. The length of the
interval is not specified and differs for each platform.

Afterwards the test will connect each on-board button to an LED (if available),
so that that LED state mirrors the button state.

Background
==========
Running this test shows if all the direct access macros for all on-board LEDs
are working correctly (xx*ON, xx*OFF, xx_TOGGLE). This test is intentionally not
using any timers, so it can also be used early on in the porting process, where
timers might not yet be available.
