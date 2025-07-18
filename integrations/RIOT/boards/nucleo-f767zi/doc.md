@defgroup    boards_nucleo-f767zi STM32 Nucleo-F767ZI
@ingroup     boards*common*nucleo144
@brief       Support for the STM32 Nucleo-F767ZI

## Overview

The Nucleo-F767ZI is a board from ST's Nucleo family supporting ARM Cortex-M7
STM32F767ZI microcontroller with 512KiB of RAM and 2 MiB of Flash.

## Pinout

@image html pinouts/nucleo-f207zg-and-more.svg "Pinout for the Nucleo-F767ZI (from STM user manual, UM1974, http://www.st.com/resource/en/user_manual/dm00244518.pdf, page 32)" width=50%

### MCU

| MCU          | STM32F767ZI
|:-------------|:--------------------|
| Family       | ARM Cortex-M7       |
| Vendor       | ST Microelectronics |
| RAM          | 512KiB              |
| Flash        | 2MiB                |
| Frequency    | up to 216 MHz       |
| FPU          | yes                 |
| Ethernet     | 10/100 Mbps         |
| Timers       | 18 (2x watchdog, 1 SysTick, 2x 32-bit, 13x 16-bit) |
| ADCs         | 3x 12 bit (up to 24 channels) |
| UARTs        | 4                   |
| I2Cs         | 4                   |
| SPIs         | 6                   |
| CAN          | 3                   |
| RTC          | 1                   |
| Datasheet    | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f767zi.pdf)|
| Reference Manual | [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0410-stm32f76xxx-and-stm32f77xxx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)|
| Programming Manual | [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0253-stm32f7-series-and-stm32h7-series-cortexm7-processor-programming-manual-stmicroelectronics.pdf)|
| Board Manual | [Board Manual](https://www.st.com/resource/en/user_manual/dm00244518-stm32-nucleo-144-boards-stmicroelectronics.pdf)|

## Flashing the Board Using ST-LINK Removable Media

On-board ST-LINK programmer provides via composite USB device removable media.
Copying the HEX file causes reprogramming of the board. This task
could be performed manually; however, the cpy2remed (copy to removable
media) PROGRAMMER script does this automatically. To program board in
this manner, use the command:
```
make BOARD=nucleo-f767zi PROGRAMMER=cpy2remed flash
```
@note This PROGRAMMER was tested using ST-LINK firmware 2.37.26. Firmware updates
      can be found on [this STM webpage](https://www.st.com/en/development-tools/stsw-link007.html).
