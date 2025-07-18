| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |
# ULP-RISC-V simple example with GPIO Interrupt:

This example demonstrates how to program the ULP-RISC-V coprocessor to wake up from a RTC IO interrupt, instead of waking periodically from the ULP timer.

ULP program written in C can be found across `ulp/main.c`. The build system compiles and links this program, converts it into binary format, and embeds it into the .rodata section of the ESP-IDF application.

At runtime, the application running inside the main CPU loads ULP program into the `RTC*SLOW*MEM` memory region using `ulp*riscv*load*binary` function. The main code then configures the ULP GPIO wakeup source and starts the coprocessor by using `ulp*riscv*config*and_run` followed by putting the chip into deep sleep mode.

When the wakeup source pin is pulled low the ULP-RISC-V coprocessor is woken up, sends a wakeup signal to the main CPU and goes back to sleep again.

In this example the input signal is connected to GPIO0. Note that this pin was chosen because most development boards have a button connected to it, so the pulses to be counted can be generated by pressing the button. For real world applications this is not a good choice of a pin, because GPIO0 also acts as a bootstrapping pin. To change the pin number, check the Chip Pin List document and adjust `WAKEUP_PIN` variable in main.c.


## Example output

```
Not a ULP-RISC-V wakeup, initializing it!
Entering in deep sleep

...

ULP-RISC-V woke up the main CPU!
Entering in deep sleep
```
