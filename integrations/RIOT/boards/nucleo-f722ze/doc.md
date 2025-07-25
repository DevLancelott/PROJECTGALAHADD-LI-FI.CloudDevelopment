@defgroup    boards_nucleo-f722ze STM32 Nucleo-F722ZE
@ingroup     boards*common*nucleo144
@brief       Support for the STM32 Nucleo-F722ZE

## Overview

The Nucleo-F722ZE is a board from ST's Nucleo family supporting ARM Cortex-M7
STM32F722ZE microcontroller with 256KiB of RAM and 512KiB of Flash.

## Pinout

@image html pinouts/nucleo-f446ze-and-f722ze.svg "Pinout for the Nucleo-F722ZE (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 35)" width=50%

### MCU

| MCU          | STM32F722ZE
|:-------------|:--------------------|
| Family       | ARM Cortex-M7       |
| Vendor       | ST Microelectronics |
| RAM          | 256KiB              |
| Flash        | 512KiB              |
| Frequency    | up to 216 MHz       |
| FPU          | yes                 |
| Timers       | 18 (2x watchdog, 1 SysTick, 2x 32-bit, 13x 16-bit) |
| ADCs         | 3x 12 bit (up to 24 channels) |
| UARTs        | 4                   |
| I2Cs         | 3                   |
| SPIs         | 5                   |
| CAN          | 1                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f722ic.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0431-stm32f72xxx-and-stm32f73xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518-stm32-nucleo-144-boards-stmicroelectronics.pdf)|


## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f722ze PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).

## Accessing RIOT shell

Default RIOT shell access utilize VCP (Virtual COM Port) via USB interface,
provided by integrated ST-LINK programmer. ST-LINK is connected to the
microcontroller USART3.

The default baud rate is 115200.
